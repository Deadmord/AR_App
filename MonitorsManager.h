#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


struct MonitorData
{
    GLFWmonitor* monitor;
    int monitor_X, monitor_Y, monitor_Width, monitor_Height;
};

class MonitorsManager
{
public:
    MonitorsManager()
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

    const MonitorData getMonitor(int windowIndex) const
    {
        return monitors.at(windowIndex);
    }

public:
    std::vector<MonitorData> monitors;
private:
    int monitorsCount{ 0 };

};

