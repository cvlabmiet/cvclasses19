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

    cv::Mat frame, copy_frame;
    cv::Mat frame_gray;
    cv::Mat split_and_merge;
    cv::Mat diff, split;

    const auto origin_wnd = "origin";
    const auto demo_wnd = "demo";

    int stddev = 50;
    cv::namedWindow(demo_wnd, 1);
    // \todo choose reasonable max value
    cv::createTrackbar("stdev", demo_wnd, &stddev, 255);

    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;

        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        frame_gray.copyTo(copy_frame);
        cv::imshow(origin_wnd, frame);
        
        split_and_merge = cvlib::split_and_merge(frame_gray, stddev);
        cv::imshow(demo_wnd, split_and_merge);
        split = cvlib::only_split(copy_frame, stddev);
        cv::absdiff(split_and_merge, split, diff);
        cv::imshow("diff", diff * 20);
		
    }

    cv::destroyWindow(origin_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
