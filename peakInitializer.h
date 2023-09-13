#pragma once

#include <memory>
#include "AcquisitionWorker.h"

class peakInitializer
{
public:
	static void InitializeLibraryFindDevices();
	static std::vector<std::shared_ptr<AcquisitionWorker>> InitializeWorkers();
	static void CloseLibrary();

private:
	static std::vector<std::shared_ptr<peak::core::DeviceDescriptor>> devices;
};

