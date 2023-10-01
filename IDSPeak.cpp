#include "IDSPeak.h"

std::shared_ptr<Device> IDSPeak::m_devicePtr;
std::shared_ptr<AcquisitionWorker> IDSPeak::m_acquisitionWorker;


void IDSPeak::Initialize()
{
    // initialize IDS peak API library
    peak::Library::Initialize();
    peak::afl::library::Init();

    try
    {
        OpenDevice();

        // Create worker thread that waits for new images from the camera
        //m_acquisitionWorker = new AcquisitionWorker(m_devicePtr->DataSteam());
        m_acquisitionWorker = std::make_shared < AcquisitionWorker>(m_devicePtr->DataSteam());
        m_acquisitionWorker->Start();
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception" << e.what();
        CloseDevice();
    }
}

void IDSPeak::CloseLibrary()
{
    if (m_acquisitionWorker)
    {
        //m_acquisitionWorker->Stop();
        m_acquisitionWorker = nullptr;
    }

    CloseDevice();

    // close IDS peak API library
    peak::Library::Close();
    peak::afl::library::Exit();
}

void IDSPeak::OpenDevice()
{
    m_devicePtr = std::make_shared<Device>();
}

void IDSPeak::CloseDevice()
{
    try
    {
        if (m_devicePtr)
        {
            // if device was opened, try to stop acquisition
            m_devicePtr->StopAcquisition();
            m_devicePtr = nullptr;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception" << e.what();
    }
}