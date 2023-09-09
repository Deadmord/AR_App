#include "MonitorsManager.h"

MonitorsManager::MonitorsManager()
{
    // ----------------- Monitors state ----------------------
    GLFWmonitor** monitorsPtr = glfwGetMonitors(&monitorsCount);
    std::cout << "Monitors curently connected: " << monitorsCount << std::endl;
    if (!monitorsPtr) {
        std::cout << "No monitors were found!" << std::endl;
        glfwTerminate();
        throw std::runtime_error("No monitors were found!");   //Refactoring !!! Add tray/catch
    }

    for (int monitorIndex{ 0 }; monitorIndex < monitorsCount; monitorIndex++)
    {
        MonitorData data;
        data.monitor = *(monitorsPtr + monitorIndex);
        glfwGetMonitorWorkarea(data.monitor, &data.monitor_X, &data.monitor_Y, &data.monitor_Width, &data.monitor_Height);
        monitors.push_back(data);
    }
}

const MonitorData MonitorsManager::getMonitor(int windowIndex) const
{
    return monitors.at(windowIndex);
}
