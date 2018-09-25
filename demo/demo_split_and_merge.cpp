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
    cv::Mat split_image;
    cv::Mat merge_image;

    const auto origin_wnd = "Origin";
    const auto split_wnd = "Split";
    const auto merge_wnd = "Merge";
    const auto difference_wnd = "Difference";

    int stddev = 8;
    int minSquare = 25;
    int meanDeviation = 2;
    int scaleFactor = 20;

    cv::namedWindow(merge_wnd, 1);
    cv::createTrackbar("stddev", merge_wnd, &stddev, 128); // stddev[{0, 255}] = 127.5
    cv::createTrackbar("square", merge_wnd, &minSquare, 100);
    cv::createTrackbar("mean", merge_wnd, &meanDeviation, 10);
    cv::createTrackbar("scale", merge_wnd, &scaleFactor, 50);

    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;

        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        cv::imshow(origin_wnd, frame);

        cvlib::split_and_merge(frame_gray, split_image, merge_image, stddev, minSquare, meanDeviation, scaleFactor);
        cv::imshow(split_wnd, split_image);
        cv::imshow(merge_wnd, merge_image);
        cv::imshow(difference_wnd, abs(split_image - merge_image) * 255);
    }

    cv::destroyWindow(origin_wnd);
    cv::destroyWindow(split_wnd);
    cv::destroyWindow(merge_wnd);
    cv::destroyWindow(difference_wnd);

    return 0;
}
