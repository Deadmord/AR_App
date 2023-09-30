#include "acquisitionworker.h"

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

AcquisitionWorker::~AcquisitionWorker()
{
    Stop();
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
            inputPixelFormat, peak::ipl::PixelFormatName::BGRa8, m_imageWidth, m_imageHeight, imageCount);

        // Start acquisition
        m_dataStream->StartAcquisition();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->WaitUntilDone();
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception: " << e.what();
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
    return imageQueue.Try_pop(image);
}

cv::Mat AcquisitionWorker::ConvertPeakImageToCvMat(const peak::ipl::Image& peakImage) {
    int width = peakImage.Width();
    int height = peakImage.Height();
    auto PixelFormat = peakImage.PixelFormat();
    uint8_t* peakData = peakImage.Data();

    if (peakImage.PixelFormat() == peak::ipl::PixelFormatName::BGRa8) {
        return cv::Mat(height, width, CV_8UC4, peakData);
    }
    else if (peakImage.PixelFormat() == peak::ipl::PixelFormatName::BGR8) {
        return cv::Mat(height, width, CV_8UC3, peakData);
    }
    else {
        auto peakImg = m_imageConverter->Convert(peakImage, peak::ipl::PixelFormatName::BGRa8);
        peakData = peakImg.Data();
        return cv::Mat(height, width, CV_8UC4, peakData);
    }
}

void AcquisitionWorker::AcquisitionLoop() {
    //while (m_running)
    //{
    //    try
    //    {
    //        const auto buffer = m_dataStream->WaitForFinishedBuffer(5000);
    //        //дебайеринг
    //        const auto peakImage = peak::BufferTo<peak::ipl::Image>(buffer).ConvertTo(
    //            peak::ipl::PixelFormatName::BGRa8, peak::ipl::ConversionMode::Classic);
    //        //конвертация peak::ipl::Image в cv::Mat
    //        cv::Mat cvImage = ConvertPeakImageToCvMat(peakImage);
    //        //добавление изображения в очередь
    //        imageQueue.Push(cvImage);

    //        m_dataStream->QueueBuffer(buffer);
    //    }
    //    catch (const peak::core::TimeoutException& e)
    //    {
    //        std::cout << "Timeout Exception getting frame: " << e.what() << std::endl;
    //        return;
    //    }
    //    catch (const std::exception& e)
    //    {
    //        std::cout << "Exception getting frame: " << e.what() << std::endl;
    //        return;
    //    }
    //}

    while (m_running)
    {
        try
        {
            // Get buffer from device's datastream
            const auto buffer = m_dataStream->WaitForFinishedBuffer(5000);

            peak::ipl::Image tempImage;
            tempImage = peak::BufferTo<peak::ipl::Image>(buffer).Clone();

            imageReceived(&tempImage);

            // Create IDS peak IPL image for debayering and convert it to RGBa8 format

            // Using the image converter ...
            //QImage qImage(static_cast<int>(m_imageWidth), static_cast<int>(m_imageHeight), QImage::Format_RGB32);
            //m_imageConverter->Convert(tempImage, peak::ipl::PixelFormatName::BGRa8, qImage.bits(),
            //    static_cast<size_t>(qImage.sizeInBytes()));
            tempImage = m_imageConverter->Convert(tempImage, peak::ipl::PixelFormatName::BGRa8);

            // ... or without image converter
            // tempImage.ConvertTo(
            //     peak::ipl::PixelFormatName::BGRa8, qImage.bits(), static_cast<size_t>(qImage.byteCount()));

            // Convert peak::ipl::Image to cv::Mat
            cv::Mat cvImage(ConvertPeakImageToCvMat(tempImage));


            // Put cvImage into Queue asynchronously
            imageQueue.Push(cvImage);

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
    std::cout << "AutoFeatures thread: " << m_autoFeaturesThread.get_id() << std::endl;
    //m_autoFeaturesThread.join();
}