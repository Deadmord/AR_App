#ifndef ACQUISITIONWORKER_H
#define ACQUISITIONWORKER_H

#include <peak_ipl/peak_ipl.hpp>
#include <peak/peak.hpp>
#include <opencv2/opencv.hpp>
#include "autofeatures.h"
#include "ThreadSafeQueue.h"

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

    void SetDataStream(std::shared_ptr<peak::core::DataStream> dataStream);
    bool TryGetImage(cv::Mat& image);

    size_t getImageWidth();
    size_t getImageHeight();

private:
    //�������� ����, ����������� � ��������� ������ - ���������� ������ - ����� � �������
    void AcquisitionLoop();
    void imageReceived(const peak::ipl::Image* image);
    cv::Mat ConvertPeakImageToCvMat(const peak::ipl::Image& peakImage);

    std::shared_ptr<peak::core::DataStream> m_dataStream;
    std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice;
    std::shared_ptr<AutoFeatures> m_autoFeatures;

    bool m_running = false;

    std::thread m_acquisitionLoopthread;
    std::thread m_autoFeaturesThread;
    ThreadSafeQueue<cv::Mat> imageQueue;
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
