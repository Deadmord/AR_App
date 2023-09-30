#pragma once

#include <memory>
#include "AcquisitionWorkerOld.h"

class peakInitializer
{
public:
	static bool InitializeLibraryFindDevices();
	static std::vector<std::shared_ptr<AcquisitionWorkerOld>> InitializeAllWorkers();
	static std::shared_ptr<AcquisitionWorkerOld> InitializeWorkerById();
	static void CloseLibrary();

private:
	static std::vector<std::shared_ptr<peak::core::DeviceDescriptor>> devices;
};

