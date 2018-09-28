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
    cv::Mat frame4split;
    cv::Mat frame4split_and_merge;

    const auto origin_wnd = "origin";
    const auto demo_wnd = "demo";
    const auto split_wnd = "split";

    int stddev = 20;
    int diffmean = 10;
    cv::namedWindow(demo_wnd, 1);
    cv::namedWindow(split_wnd, 1);
    cv::createTrackbar("stdev", split_wnd, &stddev, 100);
    cv::createTrackbar("diffmean", demo_wnd, &diffmean, 100);

    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;

        std::vector<cv::Mat> rgb4split;
        std::vector<cv::Mat> rgb4split_and_merge;
        cv::split(frame, rgb4split);
        cv::split(frame, rgb4split_and_merge);

        cvlib::split_only(rgb4split[0], stddev);
        cvlib::split_only(rgb4split[1], stddev);
        cvlib::split_only(rgb4split[2], stddev);

        cvlib::split_and_merge(rgb4split_and_merge[0], stddev, diffmean);
        cvlib::split_and_merge(rgb4split_and_merge[1], stddev, diffmean);
        cvlib::split_and_merge(rgb4split_and_merge[2], stddev, diffmean);

        cv::merge(rgb4split, frame4split);
        cv::merge(rgb4split_and_merge, frame4split_and_merge);

        cv::imshow(origin_wnd, frame);
        cv::imshow(demo_wnd, frame4split_and_merge);
        cv::imshow(split_wnd, frame4split);
    }

    cv::destroyWindow(origin_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
