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

    void SetBinning(std::string selector, int64_t horizontal, int64_t vertical)
    {
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

    void SetDataStream(std::shared_ptr<peak::core::DataStream> dataStream);
    bool TryPopImage(cv::Mat& image);

    bool isRunning();

    size_t getImageWidth();
    size_t getImageHeight();

private:
    //основной цикл, выполняется в отдельном потоке - завершение буфера - снова в очередь
    void AcquisitionLoop();
    void imageReceived(const peak::ipl::Image& image);
    cv::Mat ConvertPeakImageToCvMat(const peak::ipl::Image& peakImage);
    cv::Mat ProcessImage(const peak::ipl::Image& peakImage, double scaleFactor);

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
