#ifndef DEVICE_H
#define DEVICE_H

#include <peak/peak.hpp>
#include <memory>
#include "Console.h"

class Device
{
public:
    explicit Device();

    std::shared_ptr<peak::core::DataStream> DataSteam();;
    std::shared_ptr<peak::core::NodeMap> RemoteNodeMap();

    double Framerate();

    void FindAndOpen();
    void OpenDataStream();
    void StopAcquisition();
    void LoadDefaults();
    void DisableAutoFeatures();

    bool IsMono();
    bool HasGain();

private:
    std::shared_ptr<peak::core::Device> m_device{};
    std::shared_ptr<peak::core::DataStream> m_dataStream{};
    std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice{};

    double m_frameRate;
};

#endif // DEVICE_H
