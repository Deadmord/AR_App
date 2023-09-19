#pragma once

#include <iostream>
#include <thread>
#include <peak/peak.hpp>
#include <peak_ipl/peak_ipl.hpp>
#include <peak/converters/peak_buffer_converter_ipl.hpp>
#include <opencv2/opencv.hpp>
#include "ThreadSafeQueue.h"

class AcquisitionWorker
{
public:
	//инициализация объекта под камеру
	AcquisitionWorker(std::shared_ptr<peak::core::Device> device);
	//установка потока данных для объекта
	bool SetDataStream();
	//область интереса камеры
	bool SetRoi(int64_t x, int64_t y, int64_t width, int64_t height);
	//выделение буферов для хранения данных
	bool AllocAndAnnounceBuffers();
	bool OpenCamera();
	//подготовка для захвата данных
	bool PrepareAcquisition();
	bool StartAcquisition();
	//запуск цикла в потоке
	void Start();
	void Stop();

	bool TryGetImage(cv::Mat& image);
	cv::Mat ConvertPeakImageToCvMat(const peak::ipl::Image& peakImage);
	using ImageCallback = std::function<void(const cv::Mat& image)>;
	void SetImageCallback(ImageCallback callback);

private:
	std::shared_ptr<peak::core::Device> m_device;
	std::shared_ptr<peak::core::DataStream> m_dataStream;
	std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice;
	bool m_running = false;
	std::thread m_thread;
	ImageCallback m_imageCallback;
	ThreadSafeQueue<cv::Mat> imageQueue;
	//основной цикл, выполняется в отдельном потоке - завершение буфера - снова в очередь
	void AcquisitionLoop();
};

