/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <opencv2/opencv.hpp>

#include <cvlib.hpp>

int demo_split_and_merge(int argc, char* argv[])
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

    cv::Mat frame;
    cv::Mat frame_gray;

    const auto origin_wnd = "origin";
    const auto demo_wnd = "demo";

//    cv::namedWindow("Mask", 1);

    int stddev = 45;
    cv::namedWindow(demo_wnd, 1);
    // \todo choose reasonable max value
    cv::createTrackbar("stdev", demo_wnd, &stddev, 255);

    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;
        cv::resize(frame,frame,cv::Size(320,240));
        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        cv::imshow(origin_wnd, frame);
        cv::imshow(demo_wnd, cvlib::split_and_merge(frame_gray, stddev));
    }

    cv::destroyWindow(origin_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
