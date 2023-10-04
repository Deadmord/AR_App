﻿#include "acquisitionworker.h"

#include <peak_ipl/peak_ipl.hpp>

#include <peak/converters/peak_buffer_converter_ipl.hpp>

#include <cmath>
#include <cstring>
#include <iostream>
#include "config.h"


AcquisitionWorker::AcquisitionWorker(std::shared_ptr<peak::core::DataStream> dataStream)
{
    m_running = false;
    m_frameCounter = 0;
    m_errorCounter = 0;

    SetDataStream(dataStream);
    CreateAutoFeatures();
    InitAutoFeatures();
    m_imageConverter = std::make_unique<peak::ipl::ImageConverter>();
}

void AcquisitionWorker::Start() {
    try {
        // Use the helper function for safe node finding
        auto tlParamsLockedNode = FindNodeSafe<peak::core::nodes::IntegerNode>("TLParamsLocked");
        if (tlParamsLockedNode) {
            tlParamsLockedNode->SetValue(1);
        }

        auto widthNode = FindNodeSafe<peak::core::nodes::IntegerNode>("Width");
        if (widthNode) {
            m_bufferWidth = widthNode->Value();
        }

        auto heightNode = FindNodeSafe<peak::core::nodes::IntegerNode>("Height");
        if (heightNode) {
            m_bufferHeight = heightNode->Value();
        }

        if (m_nodemapRemoteDevice->HasNode("UsableWidth")) {
            auto usableWidthNode = FindNodeSafe<peak::core::nodes::IntegerNode>("UsableWidth");
            if (usableWidthNode) {
                m_imageWidth = usableWidthNode->Value();
            }

            auto usableHeightNode = FindNodeSafe<peak::core::nodes::IntegerNode>("UsableHeight");
            if (usableHeightNode) {
                m_imageHeight = usableHeightNode->Value();
            }
        }
        else {
            m_imageWidth = m_bufferWidth;
            m_imageHeight = m_bufferHeight;
        }

        size_t imageCount = 1;
        inputPixelFormat = static_cast<peak::ipl::PixelFormatName>(
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("PixelFormat")
            ->CurrentEntry()
            ->Value());

        m_imageConverter->PreAllocateConversion(
            inputPixelFormat, peak::ipl::PixelFormatName::BGRa8, m_imageWidth, m_imageHeight, imageCount);

        m_dataStream->StartAcquisition();
        auto acquisitionStartNode = FindNodeSafe<peak::core::nodes::CommandNode>("AcquisitionStart");
        if (acquisitionStartNode) {
            acquisitionStartNode->Execute();
            acquisitionStartNode->WaitUntilDone();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        throw std::runtime_error(e.what());
    }

    m_running = true;
    m_acquisitionLoopthread = std::thread(&AcquisitionWorker::AcquisitionLoop, this);
    std::cout << "AcquisitionWorker thread: " << m_acquisitionLoopthread.get_id() << std::endl;
}

void AcquisitionWorker::Stop()
{
    m_running = false;
    m_acquisitionLoopthread.join();
    m_autoFeatures = nullptr;
}


void AcquisitionWorker::CreateAutoFeatures()
{
    m_autoFeatures = std::make_shared<AutoFeatures>(m_nodemapRemoteDevice);
    m_autoFeatures->RegisterGainCallback([&] { std::cout << "Info: GainAutoFeature Register"; });
    m_autoFeatures->RegisterExposureCallback([&] { std::cout << "Info: ExposureAutoFeature Register"; });
    m_autoFeatures->RegisterWhiteBalanceCallback([&] { std::cout << "Info: WhiteBalanceAutoFeature Register"; });
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

void AcquisitionWorker::SetDataStream(std::shared_ptr<peak::core::DataStream> dataStream)
{
    m_dataStream = std::move(dataStream);
    m_nodemapRemoteDevice = m_dataStream->ParentDevice()->RemoteDevice()->NodeMaps().at(0);
}

bool AcquisitionWorker::TryGetImage(cv::Mat& image) {
    return imageItem.tryGet(image);
}

size_t AcquisitionWorker::getImageWidth()
{
    return m_imageWidth;
}

size_t AcquisitionWorker::getImageHeight()
{
    return m_imageHeight;
}

cv::Mat AcquisitionWorker::ConvertPeakImageToCvMat(const peak::ipl::Image& peakImage) {
    // Create CV Mat image for debayering and convert it to RGB8 format
    int width = peakImage.Width();
    int height = peakImage.Height();
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

void AcquisitionWorker::AcquisitionLoop() {

    peak::ipl::Image tempImage;

    while (m_running)
    {
        try
        {
            // Get buffer from device's datastream
            const auto buffer = m_dataStream->WaitForFinishedBuffer(5000);

            
            tempImage = peak::BufferTo<peak::ipl::Image>(buffer).Clone();

            imageReceived(&tempImage);

            // Convert peak::ipl::Image to cv::Mat
            tempImage = m_imageConverter->Convert(tempImage, peak::ipl::PixelFormatName::BGR8);
            //cv::Mat cvImage(ConvertPeakImageToCvMat(tempImage));

            // Put cvImage into ThreadSafeValue asynchronously
            imageItem.push(ConvertPeakImageToCvMat(tempImage));

            // Requeue buffer
            m_dataStream->QueueBuffer(buffer);

            m_frameCounter++;
            m_autoFeaturesThread.join();
        }
        catch (const peak::core::TimeoutException& e)
        {
            m_errorCounter++;
            std::cout << "Timeout Exception getting frame: " << e.what() << std::endl;
        }
        catch (const std::exception& e)
        {
            m_errorCounter++;
            std::cout << "Exception: " << e.what();
        }
    }
}

void AcquisitionWorker::imageReceived(const peak::ipl::Image* image)
{
    // create and run in thread autoFeatures for IDS Peak camera
    m_autoFeaturesThread = std::thread([this, image]()
        {
            m_autoFeatures->ProcessImage(image);
        });
    //std::cout << "AutoFeatures thread: " << m_autoFeaturesThread.get_id() << std::endl;
}

template <typename T>
std::shared_ptr<T> AcquisitionWorker::FindNodeSafe(const std::string& nodeName)
{
    try
    {
        return m_nodemapRemoteDevice->FindNode<T>(nodeName);
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception while finding node " << nodeName << e.what() << std::endl;
        return nullptr;
    }
}
