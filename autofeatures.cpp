#include "autofeatures.h"

AutoFeatures::AutoFeatures(std::shared_ptr<peak::core::NodeMap> nodemap)
    : m_nodemapRemoteDevice(std::move(nodemap))
{
    CreateAutoManager();
    CreateAutoControllers();
}

AutoFeatures::~AutoFeatures()
{
    m_autoFeaturesManager->DestroyAllController();
}

void AutoFeatures::CreateAutoManager()
{
    m_autoFeaturesManager = std::make_unique<peak::afl::Manager>(m_nodemapRemoteDevice);
    m_autoFeaturesManager->SetGainIPL(m_gainControllerIPL);
}

void AutoFeatures::CreateAutoControllers()
{
    m_autoBrightnessController = m_autoFeaturesManager->CreateController(PEAK_AFL_CONTROLLER_TYPE_BRIGHTNESS);

    m_autoBrightnessController->RegisterComponentCallback(PEAK_AFL_CONTROLLER_BRIGHTNESS_COMPONENT_GAIN, [&] {
        if (m_gainFinishedCallback)
        {
            m_gainFinishedCallback();
        }
    });

    m_autoBrightnessController->RegisterComponentCallback(PEAK_AFL_CONTROLLER_BRIGHTNESS_COMPONENT_EXPOSURE, [&] {
        if (m_exposureFinishedCallback)
        {
            m_exposureFinishedCallback();
        }
    });

    m_autoFeaturesManager->AddController(m_autoBrightnessController);

    m_autoWhiteBalanceController = m_autoFeaturesManager->CreateController(PEAK_AFL_CONTROLLER_TYPE_WHITE_BALANCE);
    m_autoWhiteBalanceController->RegisterFinishedCallback([&] {
        if (m_whiteBalanceFinishedCallback)
        {
            m_whiteBalanceFinishedCallback();
        }
    });

    m_autoFeaturesManager->AddController(m_autoWhiteBalanceController);
}

void AutoFeatures::ProcessImage(const peak::ipl::Image& image)
{
    if (m_autoFeaturesManager->Status())
    {
        return;
    }

    try
    {
        m_autoFeaturesManager->Process(image);
    }
    catch (const std::exception& e)
    {
        Console::yellow() << "Processing image failed: " << e.what() << std::endl;
    }
}


void AutoFeatures::SetExposureMode(peak_afl_controller_automode mode)
{
    m_autoBrightnessController->BrightnessComponentSetMode(PEAK_AFL_CONTROLLER_BRIGHTNESS_COMPONENT_EXPOSURE, mode);
}

void AutoFeatures::SetGainMode(peak_afl_controller_automode mode)
{
    m_autoBrightnessController->BrightnessComponentSetMode(PEAK_AFL_CONTROLLER_BRIGHTNESS_COMPONENT_GAIN, mode);
}

void AutoFeatures::SetWhiteBalanceMode(peak_afl_controller_automode mode)
{
    m_autoWhiteBalanceController->SetMode(mode);
}

void AutoFeatures::SetSkipFrames(int skipFrames)
{
    for (const auto& controller : m_autoFeaturesManager->ControllerList())
    {
        controller->SetSkipFrames(skipFrames);
    }
}

void AutoFeatures::Reset()
{
    for (const auto& controller : m_autoFeaturesManager->ControllerList())
    {
        if (controller->IsBrightnessComponentModeSupported())
        {
            controller->BrightnessComponentSetMode(
                PEAK_AFL_CONTROLLER_BRIGHTNESS_COMPONENT_EXPOSURE, PEAK_AFL_CONTROLLER_AUTOMODE_OFF);
            controller->BrightnessComponentSetMode(
                PEAK_AFL_CONTROLLER_BRIGHTNESS_COMPONENT_GAIN, PEAK_AFL_CONTROLLER_AUTOMODE_OFF);
        }
        else
        {
            controller->SetMode(PEAK_AFL_CONTROLLER_AUTOMODE_OFF);
        }
    }
}

void AutoFeatures::RegisterExposureCallback(std::function<void(void)> callback)
{
    m_exposureFinishedCallback = std::move(callback);
}

void AutoFeatures::RegisterGainCallback(std::function<void(void)> callback)
{
    m_gainFinishedCallback = std::move(callback);
}

void AutoFeatures::RegisterWhiteBalanceCallback(std::function<void(void)> callback)
{
    m_whiteBalanceFinishedCallback = std::move(callback);
}
