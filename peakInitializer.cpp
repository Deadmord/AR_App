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
}

std::vector<std::shared_ptr<AcquisitionWorkerOld>> peakInitializer::InitializeAllWorkers() {

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
}

void peakInitializer::CloseLibrary() {  //сделать из этого гласса синглтонн а из этого метода деструктор
    //m_acquisitionWorkers->Stop();
    peak::Library::Close();
}