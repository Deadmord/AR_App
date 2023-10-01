#ifndef DEVICE_H
#define DEVICE_H

#include <peak/peak.hpp>

#include <memory>

class Device
{
public:
    explicit Device();

    std::shared_ptr<peak::core::DataStream> DataSteam()
    {
        return m_dataStream;
    };
    std::shared_ptr<peak::core::NodeMap> RemoteNodeMap()
    {
        return m_nodemapRemoteDevice;
    }

    double Framerate()
    {
        return m_frameRate;
    }

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
