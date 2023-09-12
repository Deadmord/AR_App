#include "AcquisitionWorker.h"

AcquisitionWorker::AcquisitionWorker(std::shared_ptr<peak::core::Device> device) : m_device(device) {
	m_running = false;
	m_nodemapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);
}

void AcquisitionWorker::AcquisitionLoop() {
    while (m_running)
    {
        try
        {
            const auto buffer = m_dataStream->WaitForFinishedBuffer(5000);
            //дебайеринг
            const auto peakImage = peak::BufferTo<peak::ipl::Image>(buffer).ConvertTo(
                peak::ipl::PixelFormatName::BGRa8, peak::ipl::ConversionMode::Classic);
            //конвертация peak::ipl::Image в cv::Mat
            cv::Mat cvImage = ConvertPeakImageToCvMat(peakImage);
            //добавление изображения в очередь
            imageQueue.Push(cvImage);

            m_dataStream->QueueBuffer(buffer);
        }
        catch (const peak::core::TimeoutException& e)
        {
            std::cout << "Timeout Exception getting frame: " << e.what() << std::endl;
            return;
        }
        catch (const std::exception& e)
        {
            std::cout << "Exception getting frame: " << e.what() << std::endl;
            return;
        }
    }
}

bool AcquisitionWorker::SetDataStream(std::shared_ptr<peak::core::DataStream> dataStream) {
	m_dataStream = dataStream;
	return true;
}

bool AcquisitionWorker::SetRoi(int64_t x, int64_t y, int64_t width, int64_t height) {
    try
    {
        // Get the minimum ROI and set it. After that there are no size restrictions anymore
        int64_t x_min = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("OffsetX")->Minimum();
        int64_t y_min = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("OffsetY")->Minimum();
        int64_t w_min = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Width")->Minimum();
        int64_t h_min = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Height")->Minimum();

        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("OffsetX")->SetValue(x_min);
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("OffsetY")->SetValue(y_min);
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Width")->SetValue(w_min);
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Height")->SetValue(h_min);

        // Get the maximum ROI values
        int64_t x_max = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("OffsetX")->Maximum();
        int64_t y_max = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("OffsetY")->Maximum();
        int64_t w_max = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Width")->Maximum();
        int64_t h_max = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Height")->Maximum();

        if ((x < x_min) || (y < y_min) || (x > x_max) || (y > y_max))
        {
            return false;
        }
        else if ((width < w_min) || (height < h_min) || ((x + width) > w_max) || ((y + height) > h_max))
        {
            return false;
        }
        else
        {
            // Now, set final AOI
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("OffsetX")->SetValue(x);
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("OffsetY")->SetValue(y);
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Width")->SetValue(width);
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Height")->SetValue(height);

            return true;
        }
    }
    catch (std::exception& e)
    {
        std::cout << "Exception setting ROI: " << e.what() << std::endl;
    }

    return false;
}

bool AcquisitionWorker::AllocAndAnnounceBuffers() {
    try
    {
        if (m_dataStream)
        {
            // Flush queue and prepare all buffers for revoking
            m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);

            // Clear all old buffers
            for (const auto& buffer : m_dataStream->AnnouncedBuffers())
            {
                m_dataStream->RevokeBuffer(buffer);
            }

            int64_t payloadSize = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")->Value();

            // Get number of minimum required buffers
            int numBuffersMinRequired = m_dataStream->NumBuffersAnnouncedMinRequired();

            // Alloc buffers
            for (size_t count = 0; count < numBuffersMinRequired; count++)
            {
                auto buffer = m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
                m_dataStream->QueueBuffer(buffer);
            }

            return true;
        }
    }
    catch (std::exception& e)
    {
        std::cout << "Exception allocating buffers: " << e.what() << std::endl;
    }

    return false;
}

bool AcquisitionWorker::OpenCamera() {
    try {
        if (!m_device) {
            return false;
        }

        m_nodemapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);

        return true;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception opening camera: " << e.what() << std::endl;
        return false;
    }

    return false;
}

bool AcquisitionWorker::PrepareAcquisition() {
    try
    {
        auto dataStreams = m_device->DataStreams();
        if (dataStreams.empty())
        {
            // no data streams available
            return false;
        }

        m_dataStream = m_device->DataStreams().at(0)->OpenDataStream();

        return true;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception preparering acquisition: " << e.what() << std::endl;
    }

    return false;
}

bool AcquisitionWorker::StartAcquisition() {
    try
    {
        m_dataStream->StartAcquisition(peak::core::AcquisitionStartMode::Default, peak::core::DataStream::INFINITE_NUMBER);
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(1);
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();

        return true;
    }
    catch (std::exception& e)
    {
        std::cout << "Exception starting acquisition: " << e.what() << std::endl;
    }

    return false;
}

void AcquisitionWorker::Start() {
    m_running = true;
    m_thread = std::thread(&AcquisitionWorker::AcquisitionLoop, this);
    std::cout << "AcquisitionWorker thread: " << m_thread.get_id() << std::endl;
}

void AcquisitionWorker::Stop() {
    m_running = false;
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void AcquisitionWorker::SetImageCallback(ImageCallback callback) {
    m_imageCallback = callback;
}

bool AcquisitionWorker::TryGetImage(cv::Mat& image) {
    return imageQueue.Try_pop(image);
}

cv::Mat AcquisitionWorker::ConvertPeakImageToCvMat(const peak::ipl::Image& peakImage) {
    int width = peakImage.Width();
    int height = peakImage.Height();
    uint8_t* peakData = peakImage.Data();
    cv::Mat cvImage;

    if (peakImage.PixelFormat() == peak::ipl::PixelFormatName::BGRa8) {
        cvImage = cv::Mat(height, width, CV_8UC4, peakData);
    }
    else if (peakImage.PixelFormat() == peak::ipl::PixelFormatName::BGR8) {
        cvImage = cv::Mat(height, width, CV_8UC3, peakData);
    }
    return cvImage;
}