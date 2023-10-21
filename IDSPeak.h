#pragma once
#include "acquisitionworker.h"
#include "device.h"

#include <peak_afl/peak_afl.hpp>
#include <peak_ipl/peak_ipl.hpp>

class IDSPeak
{
public:
	static void Initialize();
	static void CloseLibrary();
	static std::shared_ptr<AcquisitionWorker> GetWorker();;

private:
	static void OpenDevice();
	static void CloseDevice();

	static std::shared_ptr<Device> m_devicePtr;
	static std::shared_ptr<AcquisitionWorker> m_acquisitionWorker;
};

