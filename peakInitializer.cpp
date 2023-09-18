#include "peakInitializer.h"

std::vector<std::shared_ptr<peak::core::DeviceDescriptor>> peakInitializer::devices;

void peakInitializer::InitializeLibraryFindDevices() {
    peak::Library::Initialize();

    auto& deviceManager = peak::DeviceManager::Instance();
    deviceManager.Update();
    devices = deviceManager.Devices();
    if (devices.empty()) {
        std::cout << "Exception: no devices found." << std::endl;
        peak::Library::Close();
        return;
    }
}

std::vector<std::shared_ptr<AcquisitionWorker>> peakInitializer::InitializeWorkers() {
    std::vector<std::shared_ptr<AcquisitionWorker>> workers;
    for (const auto& device : devices) {
        if (device->IsOpenable()) {
            auto worker = std::make_shared<AcquisitionWorker>(device->OpenDevice(peak::core::DeviceAccessType::Control));
            workers.push_back(worker);
        }
    }
    return workers;
}

void peakInitializer::CloseLibrary() {
    peak::Library::Close();
}