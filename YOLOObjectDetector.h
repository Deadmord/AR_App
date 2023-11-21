#pragma once
#include <fstream>
#include <thread>
#include <opencv2/opencv.hpp>
#include "ThreadSafeValue.h"
#include "Console.h"
#include "RTCounter.h"

class YOLOObjectDetector
{
    // Constants.
    const float INPUT_WIDTH = 640.0f;
    const float INPUT_HEIGHT = 640.0f;
    const float SCORE_THRESHOLD = 0.5f;
    const float NMS_THRESHOLD = 0.45f;
    const float CONFIDENCE_THRESHOLD = 0.45f;

    // Text parameters.
    const float FONT_SCALE = 0.6f;
    const int FONT_FACE = cv::FONT_HERSHEY_SIMPLEX;
    const int THICKNESS_1 = 1;
    const int THICKNESS_2 = 2;

    // Colors.
    const cv::Scalar BLACK = cv::Scalar(0, 0, 0);
    const cv::Scalar BLUE = cv::Scalar(255, 178, 50);
    const cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
    const cv::Scalar RED = cv::Scalar(0, 0, 255);

public:
    YOLOObjectDetector(const std::string& classListPath, const std::string& modelPath) : m_running(false)
    {
        try {
			// Load class list.
			std::ifstream ifs(classListPath);
			std::string line;
			while (getline(ifs, line))
			{
				m_classList.push_back(line);
			}

			// Load model.
			m_net = cv::dnn::readNet(modelPath);
            m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
            m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
            startThread();
		}
		catch (const std::exception& e)
		{
			Console::red() << "YOLOObjectDetector::YOLOObjectDetector exception: " << e.what() << std::endl;
        }
    }

    ~YOLOObjectDetector()
    {
        stopThread();
    }

    void processFrame(const cv::Mat& frameIn)
    {
        try
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_currentFrame.push(frameIn.getUMat(cv::ACCESS_RW));
            lock.unlock();
            m_loopCondition.notify_one();
        }
        catch (const std::exception& e)
        {
            Console::red() << "YOLOObjectDetector::processFrame exception: " << e.what() << std::endl;
        }
    }

    void showObjects(cv::Mat& frameOut)
    {
        try
        {
			std::vector<cv::Mat> detections;
            if (m_detections.tryGet(detections))
            {
                // Draw the predicted bounding box.
				post_process(frameOut, detections, m_classList);

                std::string fps = cv::format("FPSyolo: %.2f ", getFPS());
                putText(frameOut, fps, cv::Point(480, 125), FONT_FACE, FONT_SCALE, BLUE, THICKNESS_2);
            }
		}
        catch (const std::exception& e)
        {
			Console::red() << "YOLOObjectDetector::showObjects exception: " << e.what() << std::endl;
		}
	}

    float getFPS()
    {
        // Put efficiency information.
        // The function getPerfProfile returns the overall time for inference(t) and the timings for each of the layers(in layersTimes)
        std::vector<double> layersTimes;
        double freq = cv::getTickFrequency();
        return static_cast<float>(freq / m_net.getPerfProfile(layersTimes));
    }

private:
    void startThread()
    {
        try
        {
            if (!m_objectDetectorLoopThread.joinable()) {
                m_running = true;
                m_objectDetectorLoopThread = std::thread(&YOLOObjectDetector::detectionLoop, this);
                Console::green() << "YOLO detectionLoop started" << "\tLoopThread: " << m_objectDetectorLoopThread.get_id() << "\tThreadWrapperPtr: " << this << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            Console::red() << "YOLOObjectDetector::StartThread exception: " << e.what() << std::endl;
        }
    }

    void stopThread()
    {
        try
        {
            m_running = false;
            m_loopCondition.notify_one();
            if (m_objectDetectorLoopThread.joinable()) {
                Console::yellow() << "YOLO detectionLoop stopped" << "\tLoopThread: " << m_objectDetectorLoopThread.get_id() << "\tThreadWrapperPtr: " << this << std::endl;
                m_objectDetectorLoopThread.join();
            }
        }
        catch (const std::exception& e)
        {
            Console::red() << "YOLOObjectDetector::StopThread: " << e.what() << std::endl;
        }
    }

    void detectionLoop()
    {
        try
        {
            while (m_running)
            {
                cv::UMat frameToProcess;
                if (m_currentFrame.tryPop(frameToProcess)) 
                {
                    m_detections.push(pre_process(frameToProcess, m_net));
                }
                else
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_loopCondition.wait(lock);
                }
            }
        }
        catch (const std::exception& e)
        {
            Console::red() << "CascadeObjectDetector::detectionLoop: " << e.what() << std::endl;
        }
    }

    // Draw the predicted bounding box.
    void draw_label(cv::Mat& input_image, std::string label, int left, int top)
    {
        // Display the label at the top of the bounding box.
        int baseLine;
        cv::Size label_size = cv::getTextSize(label, FONT_FACE, FONT_SCALE, THICKNESS_1, &baseLine);
        top = std::max(top, label_size.height);
        // Top left corner.
        cv::Point tlc = cv::Point(left, top);
        // Bottom right corner.
        cv::Point brc = cv::Point(left + label_size.width, top + label_size.height + baseLine);
        // Draw black rectangle.
        rectangle(input_image, tlc, brc, BLACK, cv::FILLED);
        // Put the label on the black rectangle.
        putText(input_image, label, cv::Point(left, top + label_size.height), FONT_FACE, FONT_SCALE, YELLOW, THICKNESS_1);
    }

    std::vector<cv::Mat> pre_process(const cv::UMat& input_image, cv::dnn::Net& net)
    {
        // Convert to blob.
        cv::UMat blob;
        cv::dnn::blobFromImage(input_image, blob, 1. / 255., cv::Size(static_cast<size_t>(INPUT_WIDTH), static_cast<size_t>(INPUT_HEIGHT)), cv::Scalar(), true, false);

        net.setInput(blob);

        // Forward propagate.
        std::vector<cv::Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());

        return outputs;
    }

    void post_process(cv::Mat& input_image, std::vector<cv::Mat>& outputs, const std::vector<std::string>& class_name)
    {
        // Initialize vectors to hold respective outputs while unwrapping detections.
        std::vector<int> class_ids;
        std::vector<float> confidences;
        std::vector<cv::Rect> boxes;

        // Resizing factor.
        float x_factor = input_image.cols / INPUT_WIDTH;
        float y_factor = input_image.rows / INPUT_HEIGHT;

        float* data = (float*)outputs[0].data;

        const int dimensions = 85;
        const int rows = 25200;
        // Iterate through 25200 detections.
        for (int i = 0; i < rows; ++i)
        {
            float confidence = data[4];
            // Discard bad detections and continue.
            if (confidence >= CONFIDENCE_THRESHOLD)
            {
                float* classes_scores = data + 5;
                // Create a 1x85 Mat and store class scores of 80 classes.
                cv::Mat scores(1, static_cast<int>(class_name.size()), CV_32FC1, classes_scores);
                // Perform minMaxLoc and acquire index of best class score.
                cv::Point class_id;
                double max_class_score;
                minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
                // Continue if the class score is above the threshold.
                if (max_class_score > SCORE_THRESHOLD)
                {
                    // Store class ID and confidence in the pre-defined respective vectors.

                    confidences.push_back(confidence);
                    class_ids.push_back(class_id.x);

                    // Center.
                    float cx = data[0];
                    float cy = data[1];
                    // Box dimension.
                    float w = data[2];
                    float h = data[3];
                    // Bounding box coordinates.
                    int left = int((cx - 0.5 * w) * x_factor);
                    int top = int((cy - 0.5 * h) * y_factor);
                    int width = int(w * x_factor);
                    int height = int(h * y_factor);
                    // Store good detections in the boxes vector.
                    boxes.push_back(cv::Rect(left, top, width, height));
                }

            }
            // Jump to the next column.
            data += 85;
        }

        // Perform Non Maximum Suppression and draw predictions.
        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, indices);
        for (int i = 0; i < indices.size(); i++)
        {
            int idx = indices[i];
            cv::Rect box = boxes[idx];

            int left = box.x;
            int top = box.y;
            int width = box.width;
            int height = box.height;
            // Draw bounding box.
            rectangle(input_image, cv::Point(left, top), cv::Point(left + width, top + height), BLUE, 3 * THICKNESS_1);

            // Get the label for the class name and its confidence.
            std::string label = cv::format("%.2f", confidences[idx]);
            label = class_name[class_ids[idx]] + ":" + label;
            // Draw class labels.
            draw_label(input_image, label, left, top);
        }
    }

private:
    bool m_running = false;
    std::vector<std::string> m_classList;         // List of class names.
    cv::dnn::Net m_net;                           // Neural network.
    ThreadSafeValue<cv::UMat> m_currentFrame;    // Current frame.
    ThreadSafeValue<std::vector<cv::Mat>> m_detections;// Detections.
    std::thread m_objectDetectorLoopThread;     // Thread for detection loop.
    std::mutex m_mutex;							// Mutex for synchronizing access
    std::condition_variable m_loopCondition;	// Conditional variable for waiting for data
};

