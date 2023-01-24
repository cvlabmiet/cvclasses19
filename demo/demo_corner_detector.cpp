#include "utils.hpp"
#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

int demo_corner_detector(int argc, char* argv[])
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

    const auto main_wnd = "orig";
    const auto demo_wnd = "demo";

    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);

    cv::Mat frame;
    auto detector = cvlib::corner_detector_fast::create();

    utils::fps_counter fps;
    while (cv::waitKey(30) != 27)
    {
        cap >> frame;
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        cv::imshow(main_wnd, frame);
        cv::blur(frame, frame, cv::Size(5, 5));
        detector->detect(frame, corners);
        cv::drawKeypoints(frame, corners, frame, cv::Scalar(0, 0, 255));
        utils::put_fps_text(frame, fps);
        std::stringstream ss;
        ss << "Corners: " << corners.size();
        cv::putText(frame, ss.str(), cv::Point(20, 20), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 255, 0));
        cv::imshow(demo_wnd, frame);
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}