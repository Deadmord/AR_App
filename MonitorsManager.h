#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Console.h"


struct MonitorData
{
    GLFWmonitor* monitor;
    int monitor_X, monitor_Y, monitor_Width, monitor_Height;
};

class MonitorsManager
{
public:
    MonitorsManager();

    const MonitorData getMonitor(int windowIndex) const;

public:
    std::vector<MonitorData> monitors;
private:
    int monitorsCount{ 0 };

};

