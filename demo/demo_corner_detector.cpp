/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

#include "utils.hpp"

namespace
{
void trackbar_callback_func(int threshold, void* obj)
{
	cvlib::corner_detector_fast* detector = (cvlib::corner_detector_fast*)obj;
	detector->set_threshold(threshold);
}
}; // namespace

int demo_corner_detector(int argc, char* argv[])
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

    const auto main_wnd = "main";
    const auto demo_wnd = "demo";

    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);

    cv::Mat frame;
    auto detector = cvlib::corner_detector_fast::create();
    std::vector<cv::KeyPoint> corners;

	int threshold = 20;
	cv::createTrackbar("thresh", demo_wnd, &threshold, 255, trackbar_callback_func, (void*)detector);
	detector->set_threshold(threshold);

    utils::fps_counter fps;
    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;
        cv::imshow(main_wnd, frame);

        detector->detect(frame, corners);
        cv::drawKeypoints(frame, corners, frame, cv::Scalar(0, 0, 255));
        utils::put_fps_text(frame, fps);
		cv::putText(frame, std::to_string(corners.size()),
				cv::Point(10, 20), CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0));
        cv::imshow(demo_wnd, frame);
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
