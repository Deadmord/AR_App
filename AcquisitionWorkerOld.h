#pragma once

#include <iostream>
#include <thread>
#include <peak/peak.hpp>
#include <peak_ipl/peak_ipl.hpp>
#include <peak_afl/peak_afl.hpp>
#include <peak/converters/peak_buffer_converter_ipl.hpp>
#include <opencv2/opencv.hpp>
#include "ThreadSafeQueue.h"

class AcquisitionWorkerOld
{
public:
	//������������� ������� ��� ������
	AcquisitionWorkerOld(std::shared_ptr<peak::core::Device> device);
	//��������� ������ ������ ��� �������
	bool SetDataStream(std::shared_ptr<peak::core::DataStream> dataStream);
	//������� �������� ������
	bool SetRoi(int64_t x, int64_t y, int64_t width, int64_t height);
	//��������� ������� ��� �������� ������
	bool AllocAndAnnounceBuffers();
	bool OpenCamera();
	//���������� ��� ������� ������
	bool PrepareAcquisition();
	bool StartAcquisition();
	//������ ����� � ������
	void Start();
	void Stop();
	
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
	//�������� ����, ����������� � ��������� ������ - ���������� ������ - ����� � �������
	void AcquisitionLoop();
};

