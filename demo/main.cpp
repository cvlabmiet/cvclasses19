/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <cvlib.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]) {
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

    cv::Mat frame;
    cv::Mat frame_gray1;
    cv::Mat frame_gray2;

    const auto origin_wnd = "origin";
    const auto split_wnd = "split";
    const auto merge_wnd = "merge";

    int stddev = 50;
    cv::namedWindow(split_wnd, 1);
    cv::namedWindow(merge_wnd, 1);

    int a = 0;

    cv::createTrackbar("stdev", split_wnd, &stddev, 255);

    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;

        std::vector<cv::Mat> rgb;
        cv::cvtColor(frame, frame_gray1, cv::COLOR_BGR2GRAY);
        frame_gray2 = frame_gray1.clone();

        cv::imshow(origin_wnd, frame);
        cv::imshow(split_wnd, cvlib::cvlib_split(frame_gray1, stddev));
        cv::imshow(merge_wnd, cvlib::cvlib_merge(frame_gray2, stddev));
    }

    return 0;
}
