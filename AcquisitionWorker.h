#ifndef ACQUISITIONWORKER_H
#define ACQUISITIONWORKER_H

#include <peak_ipl/peak_ipl.hpp>
#include <peak/peak.hpp>
#include <opencv2/opencv.hpp>
#include "autofeatures.h"
#include "ThreadSafeValue.h"
#include "Console.h"

class AcquisitionWorker
{

public:
    AcquisitionWorker(std::shared_ptr<peak::core::DataStream> dataStream);
    ~AcquisitionWorker() = default;

    void Start();
    void Stop();
    void CreateAutoFeatures();
    void InitAutoFeatures();
    void ResetAutoFeatures();

    size_t getImageWidth();
    size_t getImageHeight();
    void setImageWidth(size_t width);
    void setImageHeight(size_t height);
    void SetBinning(bool enable, std::string selector, int64_t horizontal, int64_t vertical);

    void SetDataStream(std::shared_ptr<peak::core::DataStream> dataStream);
    bool TryPopImage(cv::Mat& image);

    bool isRunning();

private:
    //основной цикл, выполняется в отдельном потоке - завершение буфера - снова в очередь
    void AcquisitionLoop();
    void imageReceived(const peak::ipl::Image& image);
    cv::Mat resizeFrame(const cv::Mat& image);
    cv::Mat ConvertPeakImageToCvMat(const peak::ipl::Image& peakImage);


    std::shared_ptr<peak::core::DataStream> m_dataStream;
    std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice;
    std::shared_ptr<AutoFeatures> m_autoFeatures;

    bool m_running = false;

    std::thread m_acquisitionLoopthread;
    std::thread m_autoFeaturesThread;
    ThreadSafeValue<cv::Mat> imageItem;
    unsigned int m_frameCounter = 0;
    unsigned int m_errorCounter = 0;

    peak::ipl::PixelFormatName inputPixelFormat;
    size_t m_imageWidth = 0;
    size_t m_imageHeight = 0;
    size_t m_bufferWidth = 0;
    size_t m_bufferHeight = 0;

    std::unique_ptr<peak::ipl::ImageConverter> m_imageConverter;
};

#endif // ACQUISITIONWORKER_H
