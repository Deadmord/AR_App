#include "acquisitionworker.h"

#include <peak_ipl/peak_ipl.hpp>

#include <peak/converters/peak_buffer_converter_ipl.hpp>

#include <cmath>
#include <cstring>
#include <iostream>
#include "config.h"

#include <future>

// Define a flag to indicate a timeout.
std::atomic<bool> timeoutOccurred(false);

AcquisitionWorker::AcquisitionWorker(std::shared_ptr<peak::core::DataStream> dataStream)
{
    m_running = false;
    m_frameCounter = 0;
    m_errorCounter = 0;

    SetDataStream(dataStream);
    SetBinning(enableBinning, selector, horizontal, vertical);
    CreateAutoFeatures();
    InitAutoFeatures();
    m_imageConverter = std::make_unique<peak::ipl::ImageConverter>();
}

void AcquisitionWorker::Start()
{
    try
    {
        // Lock critical features to prevent them from changing during acquisition
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(1);

        // Determine RAW buffer size
        m_bufferWidth = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Width")->Value();
        m_bufferHeight = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Height")->Value();

        // Determine image size
        if (m_nodemapRemoteDevice->HasNode("UsableWidth"))
        {
            m_imageWidth = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("UsableWidth")
                               ->Value();
            m_imageHeight = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("UsableHeight")
                                ->Value();
        }
        else
        {
            m_imageWidth = m_bufferWidth;
            m_imageHeight = m_bufferHeight;
        }

        // Pre-allocate images for conversion that can be used simultaneously
        // This is not mandatory but it can increase the speed of image conversions
        size_t imageCount = 1;
        inputPixelFormat = static_cast<peak::ipl::PixelFormatName>(
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("PixelFormat")
                ->CurrentEntry()
                ->Value());

        m_imageConverter->PreAllocateConversion(
            inputPixelFormat, peak::ipl::PixelFormatName::BGR8, m_imageWidth, m_imageHeight, imageCount);

        // Start acquisition
        m_dataStream->StartAcquisition();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->WaitUntilDone();
    }
    catch (const std::exception& e)
    {
        Console::red() << "AcquisitionWorker::Start Exception: " << e.what() << std::endl;
        throw std::runtime_error(e.what());
    }

    m_running = true;

    m_acquisitionLoopthread = std::thread(&AcquisitionWorker::AcquisitionLoop, this);
    Console::yellow() << "IDS AcquisitionWorker thread start: " << m_acquisitionLoopthread.get_id() << std::endl;
}

void AcquisitionWorker::Stop()
{
    Console::yellow() << "IDS AcquisitionWorker thread stop: " << m_acquisitionLoopthread.get_id() << std::endl;
    m_running = false;
    m_acquisitionLoopthread.join();
    m_autoFeatures = nullptr;
}


void AcquisitionWorker::CreateAutoFeatures()
{
    m_autoFeatures = std::make_shared<AutoFeatures>(m_nodemapRemoteDevice);
    m_autoFeatures->RegisterGainCallback([&] { Console::log() << "Info: GainAutoFeature Register" << std::endl; });
    m_autoFeatures->RegisterExposureCallback([&] { Console::log() << "Info: ExposureAutoFeature Register" << std::endl; });
    m_autoFeatures->RegisterWhiteBalanceCallback([&] { Console::log() << "Info: WhiteBalanceAutoFeature Register" << std::endl; });
}

void AcquisitionWorker::InitAutoFeatures()
{
    m_autoFeatures->SetExposureMode(static_cast<peak_afl_controller_automode>(exposureAutoFeatures));
    m_autoFeatures->SetGainMode(static_cast<peak_afl_controller_automode>(gainAutoFeatures));
    m_autoFeatures->SetWhiteBalanceMode(static_cast<peak_afl_controller_automode>(balanceWhiteAutoFeatures));
    m_autoFeatures->SetSkipFrames(skip_frames);
}

inline void AcquisitionWorker::ResetAutoFeatures()
{
    m_autoFeatures->Reset();
}

void AcquisitionWorker::SetBinning(bool enable, std::string selector, int64_t horizontal, int64_t vertical)
{
    if (!enable) return;
    try
    {
        if (false)
        {
            Console::log() << "SetBinning " << std::endl;
            // Determine the current entry of BinningSelector
            std::string value = m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("BinningSelector")->CurrentEntry()->SymbolicValue();
            // Get a list of all available entries of BinningSelector
            auto allEntriesBinningSelector = m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("BinningSelector")->Entries();
            std::vector<std::shared_ptr<peak::core::nodes::EnumerationEntryNode>> availableEntriesBinningSelector;
            for (const auto& entry : allEntriesBinningSelector)
            {
                if ((entry->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotAvailable)
                    && (entry->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotImplemented))
                {
                    Console::log() << "BinningSelector " << entry->SymbolicValue() << " is available." << std::endl;
                    availableEntriesBinningSelector.emplace_back(entry);
                }
            }
            // Determine the current BinningHorizontal
            int64_t BinningHorizontalValue = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("BinningHorizontal")->Value();
            // Determine the current BinningHorizontal
            int64_t BinningVerticalValue = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("BinningVertical")->Value();

            // Determine the current entry of BinningHorizontalMode
            // std::string BinningHorizontalModeValue = m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("BinningHorizontalMode")->CurrentEntry()->SymbolicValue();
            // Get a list of all available entries of BinningHorizontalMode
            auto allEntriesBinningMode = m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("BinningHorizontalMode")->Entries();
            std::vector<std::shared_ptr<peak::core::nodes::EnumerationEntryNode>> availableEntriesBinningMode;
            for (const auto& entry : allEntriesBinningMode)
            {
                if ((entry->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotAvailable)
                    && (entry->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotImplemented))
                {
                    Console::log() << "BinningSelector " << entry->SymbolicValue() << " is available." << std::endl;
                    availableEntriesBinningMode.emplace_back(entry);
                }
            }
        }

        // Before accessing BinningHorizontal, make sure BinningSelector is set correctly
        // Set BinningSelector
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("BinningSelector")->SetCurrentEntry(selector);
        // Set BinningHorizontal
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("BinningHorizontal")->SetValue(horizontal);
        // Set BinningVertical
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("BinningVertical")->SetValue(vertical);

    }
    catch (const std::exception& e)
    {
        Console::red() << "AcquisitionWorker::SetBinning Exception: " << e.what() << std::endl;
        throw std::runtime_error(e.what());
    }
}

void AcquisitionWorker::SetDataStream(std::shared_ptr<peak::core::DataStream> dataStream)
{
    m_dataStream = std::move(dataStream);
    m_nodemapRemoteDevice = m_dataStream->ParentDevice()->RemoteDevice()->NodeMaps().at(0);
}

bool AcquisitionWorker::TryPopImage(cv::Mat& image) {
    if (m_running)
    {
        imageItem.waitAndPop(image);
        return true;
    }
    else 
        return false;
}

bool AcquisitionWorker::isRunning()
{
    return m_running;
}

size_t AcquisitionWorker::getImageWidth()
{
    return m_imageWidth;
}

size_t AcquisitionWorker::getImageHeight()
{
    return m_imageHeight;
}

void AcquisitionWorker::setImageWidth(size_t width)
{
    Console::log() << "Frame width changed from " << m_imageWidth << " to " << width << std::endl;
    m_imageWidth = width;
}

void AcquisitionWorker::setImageHeight(size_t height)
{
    Console::log() << "Frame height changed from " << m_imageHeight << " to " << height << std::endl;
    m_imageHeight = height;
}

void AcquisitionWorker::AcquisitionLoop() {

    peak::ipl::Image tempImage;

    while (m_running)
    {
        try
        {
            // Get buffer from device's datastream
            const auto buffer = m_dataStream->WaitForFinishedBuffer(5000);

            // Clone buffer to tempImage
            tempImage = peak::BufferTo<peak::ipl::Image>(buffer).Clone();

            // call autoFeatures for new image
            imageReceived(tempImage);

            // Convert BayerGR8 to BGR8
            tempImage = m_imageConverter->Convert(tempImage, peak::ipl::PixelFormatName::BGR8);
            //cv::Mat cvImage(ConvertPeakImageToCvMat(tempImage));

            // Perform cvImage and put into ThreadSafeValue asynchronously
            imageItem.push(resizeFrame(ConvertPeakImageToCvMat(tempImage)));

            // Requeue buffer
            m_dataStream->QueueBuffer(buffer);

            m_frameCounter++;
            m_autoFeaturesThread.join();
        }
        catch (const peak::core::TimeoutException& e)
        {
            m_errorCounter++;
            Console::red() << "Timeout Exception getting frame: " << e.what() << std::endl;
        }
        catch (const std::exception& e)
        {
            m_errorCounter++;
            Console::red() << "Exception: " << e.what() << std::endl;
        }
    }
}

void AcquisitionWorker::imageReceived(const peak::ipl::Image& image)
{
    // create and run in thread autoFeatures for IDS Peak camera
    m_autoFeaturesThread = std::thread([this, image]()
        {
            m_autoFeatures->ProcessImage(image);
        });
    //Console::yellow() << "AutoFeatures thread: " << m_autoFeaturesThread.get_id() << std::endl;
}

cv::Mat AcquisitionWorker::resizeFrame(const cv::Mat& image)
{
    cv::Mat resImage;
    cv::resize(image, resImage, cv::Size(static_cast<int>(m_imageWidth), static_cast<int>(m_imageHeight)), cv::INTER_LINEAR);
    return resImage;
}

cv::Mat AcquisitionWorker::ConvertPeakImageToCvMat(const peak::ipl::Image& peakImage) {
    // Create CV Mat image for debayering and convert it to RGB8 format
    int width = static_cast<int>(peakImage.Width());
    int height = static_cast<int>(peakImage.Height());
    auto PixelFormat = peakImage.PixelFormat();
    uint8_t* peakData = peakImage.Data();

    if (peakImage.PixelFormat() == peak::ipl::PixelFormatName::BGRa8) {
        cv::Mat bgraImage(height, width, CV_8UC4, peakData);
        cv::Mat bgrImage;
        cv::cvtColor(bgraImage, bgrImage, cv::COLOR_BGRA2BGR);      //very heavy operation
        return bgrImage;
        //return cv::Mat(height, width, CV_8UC4, peakData);
    }
    else if (peakImage.PixelFormat() == peak::ipl::PixelFormatName::BGR8) {
        return cv::Mat(height, width, CV_8UC3, peakData);
    }
    else {
        auto peakImg = m_imageConverter->Convert(peakImage, peak::ipl::PixelFormatName::BGR8);
        peakData = peakImg.Data();
        return cv::Mat(height, width, CV_8UC3, peakData);
    }
}