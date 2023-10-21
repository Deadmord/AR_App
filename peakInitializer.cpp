#include "peakInitializer.h"

std::vector<std::shared_ptr<peak::core::DeviceDescriptor>> peakInitializer::devices;

bool peakInitializer::InitializeLibraryFindDevices() {

    // initialize IDS peak API library
    peak::Library::Initialize();
    peak::afl::library::Init();

    try
    {
        // Create instance of the device manager
        auto& deviceManager = peak::DeviceManager::Instance();

        // Update the device manager
        deviceManager.Update();

        // Return if no device was found
        if (deviceManager.Devices().empty())
        {
            throw std::runtime_error("Error: No device found"); 
            //Return not avalible, it throw to catch 
            return false; 
        }

        devices = deviceManager.Devices();
        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception: "<< e.what() << std::endl;
        CloseLibrary();
        return false;
    }

    return false; //Return not avalible
}

std::vector<std::shared_ptr<AcquisitionWorkerOld>> peakInitializer::InitializeAllWorkers() {
    //std::vector<std::shared_ptr<AcquisitionWorker>> workers;
    //for (const auto& device : devices) {
    //    if (device->IsOpenable()) {
    //        auto worker = std::make_shared<AcquisitionWorker>(device->OpenDevice(peak::core::DeviceAccessType::Control));
    //        workers.push_back(worker);
    //    }
    //}
    //example
    std::vector<std::shared_ptr<AcquisitionWorkerOld>> workers;
    try
    {
        // Try to open all devices in the device manager's device list
        for (const auto& device : devices) 
        {
            if (device->IsOpenable())
            {
                // Try to open all devices and check for available data streams
                auto openedDevice = device->OpenDevice(peak::core::DeviceAccessType::Control);
                if (openedDevice->DataStreams().empty())
                {
                    openedDevice.reset();
                }
                else
                {
                    //Create and add workers for all opened devices with an available data stream
                    auto worker = std::make_shared<AcquisitionWorkerOld>(openedDevice);
                    workers.push_back(worker);
                }
            }

            if (workers.empty())
                throw std::runtime_error("Error: No device with available data streams could be opened");
        }

        //if (m_device)
        //{
        //    // Open standard data stream
        //    auto dataStreams = m_device->DataStreams();
        //    if (dataStreams.empty())
        //    {
        //        emit MessageBoxTrigger("Error", "Device has no DataStream");
        //        m_device.reset();
        //        return false;
        //    }

        //    try
        //    {
        //        // Open standard data stream
        //        m_dataStream = dataStreams.at(0)->OpenDataStream();
        //    }
        //    catch (const std::exception& e)
        //    {
        //        // Open data stream failed
        //        m_device.reset();
        //        emit MessageBoxTrigger("Error", QString("Failed to open DataStream\n") + e.what());
        //        return false;
        //    }

        //    // Get nodemap of remote device for all accesses to the genicam nodemap tree
        //    m_nodemapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);

        //    // To prepare for untriggered continuous image acquisition, load the default user set if available
        //    // and wait until execution is finished
        //    try
        //    {
        //        m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")
        //            ->SetCurrentEntry("Default");

        //        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->Execute();
        //        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")
        //            ->WaitUntilDone();
        //    }
        //    catch (const peak::core::NotFoundException&)
        //    {
        //        // UserSet is not available
        //    }

        //    // Get the payload size for correct buffer allocation
        //    auto payloadSize = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")
        //        ->Value();

        //    // Get the minimum number of buffers that must be announced
        //    auto bufferCountMax = m_dataStream->NumBuffersAnnouncedMinRequired();

        //    // Allocate and announce image buffers and queue them
        //    for (size_t bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
        //    {
        //        auto buffer = m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
        //        m_dataStream->QueueBuffer(buffer);
        //    }

        //    // Configure worker
        //    m_acquisitionWorker->SetDataStream(m_dataStream);

        //    // If the camera has an internal autofocus or auto exposure mode,
        //    // it needs to be set to Off so the auto feature lib
        //    // can e.g. access the FocusStepper
        //    EnsureDeviceAutoFeaturesAreDisabled();

        //    return true;
        //}
        ////проверить что камер загружено нужное количество
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        CloseLibrary();
    }
    return workers;
}

std::shared_ptr<AcquisitionWorkerOld> peakInitializer::InitializeWorkerById()
{
    return std::shared_ptr<AcquisitionWorkerOld>{};
    //example
    // open the first openable device in the device manager's device list
    
    //size_t deviceCount = deviceManager.Devices().size();
    //for (size_t i = 0; i < deviceCount; ++i)
    //{
    //    if (deviceManager.Devices().at(i)->IsOpenable())
    //    {
    //        // Try to open all devices and check for available data streams
    //        m_device = deviceManager.Devices().at(i)->OpenDevice(peak::core::DeviceAccessType::Control);
    //        if (m_device->DataStreams().empty())
    //        {
    //            m_device.reset();
    //        }
    //        else
    //        {
    //            // stop after the first opened device with an available data stream
    //            break;
    //        }
    //    }

    //    if (i == (deviceCount - 1))
    //    {
    //        emit MessageBoxTrigger("Error", "No device with available data streams could be opened");
    //        return false;
    //    }
    //}

    //if (m_device)
    //{
    //    // Open standard data stream
    //    auto dataStreams = m_device->DataStreams();
    //    if (dataStreams.empty())
    //    {
    //        emit MessageBoxTrigger("Error", "Device has no DataStream");
    //        m_device.reset();
    //        return false;
    //    }

    //    try
    //    {
    //        // Open standard data stream
    //        m_dataStream = dataStreams.at(0)->OpenDataStream();
    //    }
    //    catch (const std::exception& e)
    //    {
    //        // Open data stream failed
    //        m_device.reset();
    //        emit MessageBoxTrigger("Error", QString("Failed to open DataStream\n") + e.what());
    //        return false;
    //    }

    //    // Get nodemap of remote device for all accesses to the genicam nodemap tree
    //    m_nodemapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);

    //    // To prepare for untriggered continuous image acquisition, load the default user set if available
    //    // and wait until execution is finished
    //    try
    //    {
    //        m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")
    //            ->SetCurrentEntry("Default");

    //        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->Execute();
    //        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")
    //            ->WaitUntilDone();
    //    }
    //    catch (const peak::core::NotFoundException&)
    //    {
    //        // UserSet is not available
    //    }

    //    // Get the payload size for correct buffer allocation
    //    auto payloadSize = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")
    //        ->Value();

    //    // Get the minimum number of buffers that must be announced
    //    auto bufferCountMax = m_dataStream->NumBuffersAnnouncedMinRequired();

    //    // Allocate and announce image buffers and queue them
    //    for (size_t bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
    //    {
    //        auto buffer = m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
    //        m_dataStream->QueueBuffer(buffer);
    //    }

    //    // Configure worker
    //    m_acquisitionWorker->SetDataStream(m_dataStream);

    //    // If the camera has an internal autofocus or auto exposure mode,
    //    // it needs to be set to Off so the auto feature lib
    //    // can e.g. access the FocusStepper
    //    EnsureDeviceAutoFeaturesAreDisabled();

    //    return true;
    //}
}

void peakInitializer::CloseLibrary() { 
    peak::Library::Close();
}