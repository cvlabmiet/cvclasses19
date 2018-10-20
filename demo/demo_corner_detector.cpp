/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

#include "utils.hpp"

void on_threshold_changed(int value, void* ptr)
{
    ((cvlib::corner_detector_fast*)(ptr))->setVarThreshold(value);
}

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
    // auto detector = cv::GFTTDetector::create(); // \todo use cvlib::corner_detector_fast
    // auto detector = cv::FastFeatureDetector::create();
    auto detector = cvlib::corner_detector_fast::create();
    std::vector<cv::KeyPoint> corners;

    int threshold = 20;
    cv::createTrackbar("th", demo_wnd, &threshold, 50, on_threshold_changed, (void*)detector);

    cv::Mat frame_gray;

    detector->setVarThreshold(threshold);
    utils::fps_counter fps;
    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;
        cv::imshow(main_wnd, frame);

        // detector->setVarThreshold(threshold);
        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        detector->detect(frame_gray, corners);

        cv::drawKeypoints(frame, corners, frame, cv::Scalar(0, 0, 255));
        utils::put_fps_text(frame, fps);
        utils::put_num_corners_text(frame, detector->getNumPoint());
        // \todo add count of the detected corners at the top left corner of the image. Use green text color.
        cv::imshow(demo_wnd, frame);
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
