#ifndef AUTOFEATURES_H
#define AUTOFEATURES_H

#include <peak_afl/peak_afl.hpp>
#include <peak_ipl/peak_ipl.hpp>
#include <peak/peak.hpp>

#include <iostream>
#include <memory>
#include <functional>
#include "Console.h"

class AutoFeatures
{
public:
    AutoFeatures(std::shared_ptr<peak::core::NodeMap> nodemap);
    ~AutoFeatures();

    void ProcessImage(const peak::ipl::Image& image);
    
    void SetExposureMode(peak_afl_controller_automode mode);
    void SetGainMode(peak_afl_controller_automode mode);
    void SetWhiteBalanceMode(peak_afl_controller_automode mode);
    void SetSkipFrames(int skipFrames);
    void Reset();

    void RegisterExposureCallback(std::function<void(void)> callback);
    void RegisterGainCallback(std::function<void(void)> callback);
    void RegisterWhiteBalanceCallback(std::function<void(void)> callback);

private:
    void CreateAutoManager();
    void CreateAutoControllers();

    std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice{};
    peak::ipl::Gain m_gainControllerIPL{};

    std::unique_ptr<peak::afl::Manager> m_autoFeaturesManager{};
    std::shared_ptr<peak::afl::Controller> m_autoBrightnessController{};
    std::shared_ptr<peak::afl::Controller> m_autoWhiteBalanceController{};

    std::function<void(void)> m_exposureFinishedCallback;
    std::function<void(void)> m_gainFinishedCallback;
    std::function<void(void)> m_whiteBalanceFinishedCallback;
};

#endif // AUTOFEATURES_H
