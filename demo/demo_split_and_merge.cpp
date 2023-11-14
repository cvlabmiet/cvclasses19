/* Demo application for Computer Vision Library.
 * @file
 * @date 2023-14-11
 * @author Yaroslav Murenkov
 */

#include <opencv2/opencv.hpp>

#include <cvlib.hpp>

int demo_split_and_merge(int argc, char* argv[])
{
    cv::VideoCapture cap(0, 200);
    if (!cap.isOpened())
        return -1;

    cv::Mat frame;
    cv::Mat frame_gray;

    const auto origin_wnd = "origin";
    const auto demo_wnd = "demo";

    int stddev = 25;
    cv::namedWindow(demo_wnd, 1);
    cv::createTrackbar("stdev", demo_wnd, &stddev, 50);

    const auto ESC_KEY_CODE = 27;
    while (cv::waitKey(30) != ESC_KEY_CODE)
    {
        cap >> frame;

        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        cv::imshow(origin_wnd, frame);
        cv::imshow(demo_wnd, cvlib::split_and_merge(frame_gray, stddev));
    }

    cv::destroyWindow(origin_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
