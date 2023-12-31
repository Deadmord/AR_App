#include "device.h"


namespace
{
bool IsAccessable(const std::shared_ptr<peak::core::nodes::Node>& node)
{
    try
    {
        auto status = node->AccessStatus();
        return (status != peak::core::nodes::NodeAccessStatus::NotAvailable && status != peak::core::nodes::NodeAccessStatus::NotImplemented);
    }
    catch (const std::exception& e)
    {
        Console::red() << "Exception IDS device IsAccessable check: " << e.what() << std::endl;
        return false;
    }
}

bool IsWriteAble(const std::shared_ptr<peak::core::nodes::Node>& node)
{
    try
    {
        auto status = node->AccessStatus();
        return (status == peak::core::nodes::NodeAccessStatus::ReadWrite || status == peak::core::nodes::NodeAccessStatus::WriteOnly);
    }
    catch (const std::exception& e)
    {
        Console::red() << "Exception IDS device IsWriteAble check: " << e.what() << std::endl;
        return false;
    }
}
} // namespace

Device::Device()
{
    FindAndOpen();
    OpenDataStream();
    LoadDefaults();
    DisableAutoFeatures();

    Console::green() << "IDS camera found. Model: " << m_deviceModel << std::endl;
}

std::shared_ptr<peak::core::DataStream> Device::DataSteam()
{
    return m_dataStream;
}

std::shared_ptr<peak::core::NodeMap> Device::RemoteNodeMap()
{
    return m_nodemapRemoteDevice;
}

double Device::Framerate()
{
    return m_frameRate;
}

void Device::FindAndOpen()
{
    auto& deviceManager = peak::DeviceManager::Instance();

    // Update the device manager
    deviceManager.Update();

    // Return if no device was found
    if (deviceManager.Devices().empty())
    {
        throw std::runtime_error("No device found");
    }

    // open the first openable device in the device manager's device list
    size_t deviceCount = deviceManager.Devices().size();
    for (size_t i = 0; i < deviceCount; ++i)
    {
        if (deviceManager.Devices().at(i)->IsOpenable())
        {
            m_device = deviceManager.Devices().at(i)->OpenDevice(peak::core::DeviceAccessType::Control);

            // stop after the first opened device
            break;
        }
        else if (i == (deviceCount - 1))
        {
            throw std::runtime_error("Device could not be opened");
        }
    }
}

void Device::OpenDataStream()
{
    if (m_device)
    {
        // Open standard data stream
        auto dataStreams = m_device->DataStreams();
        if (dataStreams.empty())
        {
            throw std::runtime_error("Device has no DataStream");
        }

        // Open standard data stream
        m_dataStream = dataStreams.at(0)->OpenDataStream();

        // Get nodemap of remote device for all accesses to the genicam nodemap tree
        m_nodemapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);

        // Get the payload size for correct buffer allocation
        auto payloadSize = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")
                               ->Value();

        // Get the minimum number of buffers that must be announced
        auto bufferCountMax = m_dataStream->NumBuffersAnnouncedMinRequired();

        // Allocate and announce image buffers and queue them
        for (size_t bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
        {
            auto buffer = m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
            m_dataStream->QueueBuffer(buffer);
        }

        m_deviceModel = m_nodemapRemoteDevice->FindNode<peak::core::nodes::StringNode>("DeviceModelName")->Value();
    }
}

void Device::LoadDefaults()
{
    // To prepare for untriggered continuous image acquisition, load the default user set if available
    // and wait until execution is finished
    try
    {
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")
            ->SetCurrentEntry("Default");

        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->Execute();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->WaitUntilDone();

        // Set exposure to minimum
        double exposure = m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("ExposureTime")
                              ->Minimum();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("ExposureTime")->SetValue(exposure);

        // Set frame rate
        const auto frameRate = (std::min)(
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")->Maximum(),
            30.0);
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")
            ->SetValue(frameRate);

        m_frameRate = m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")
                          ->Value();
    }
    catch (const peak::core::NotFoundException& e)
    {
        Console::red() << "UserSet is not available. Exception: " << e.what() << std::endl;
    }
}

void Device::DisableAutoFeatures()
{
    // Make sure that no auto feature is enabled by default
    try
    {
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("ExposureAuto")
            ->SetCurrentEntry("Off");
    }
    catch (const std::exception&)
    {
        // Ignore
    }
    try
    {
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("GainAuto")
            ->SetCurrentEntry("Off");
    }
    catch (const std::exception&)
    {
        // Ignore
    }
    try
    {
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("BalanceWhiteAuto")
            ->SetCurrentEntry("Off");
    }
    catch (const std::exception&)
    {
        // Ignore
    }
}

void Device::StopAcquisition()
{
    if (m_device)
    {
        auto remoteNodeMap = m_device->RemoteDevice()->NodeMaps().at(0);
        remoteNodeMap->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->Execute();
    }

    // if data stream was opened, try to stop it and revoke its image buffers
    if (m_dataStream)
    {
        m_dataStream->KillWait();
        m_dataStream->StopAcquisition(peak::core::AcquisitionStopMode::Default);
        m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);

        for (const auto& buffer : m_dataStream->AnnouncedBuffers())
        {
            m_dataStream->RevokeBuffer(buffer);
        }
    }
}


bool Device::IsMono()
{
    auto pixelFormat = m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("PixelFormat")
                           ->CurrentEntry()
                           ->SymbolicValue();
    return ("Mono" == pixelFormat.substr(0, 4));
}

bool Device::HasGain()
{
    try
    {
        if (m_nodemapRemoteDevice->HasNode("GainSelector"))
        {
            const auto node = m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>(
                "GainSelector");
            if (IsAccessable(node))
            {
                for (const auto& entry : node->Entries())
                {
                    for (const auto& gainType : { "AnalogAll", "DigitalAll" })
                        if (gainType == entry->StringValue())
                        {
                            if (IsAccessable(entry) && IsWriteAble(node))
                            {
                                return true;
                            }
                        }
                }
            }
        }
    }
    catch (std::exception& e)
    {
        Console::red() << "Device::HasGain Exception: " << e.what() << std::endl;
    }

    return false;
}
