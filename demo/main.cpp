/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-10
 * @author Max Kimlyk
 */

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[])
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

    cv::Mat frame;
    cv::Mat frame_gray;

    const auto origin_wnd = "origin";
    const auto demo_wnd = "demo";

    int stddev = 30;
    int min_chunk_size = 10;
    cv::namedWindow(demo_wnd, 1);
    cv::createTrackbar("stdev", demo_wnd, &stddev, 255);
    cv::createTrackbar("min chunk size", demo_wnd, &min_chunk_size, 20);

    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;
        cv::imshow(origin_wnd, frame);
        min_chunk_size = min_chunk_size == 0 ? 1 : min_chunk_size; // avoiding division by zero
        cv::imshow(demo_wnd, cvlib::split_and_merge(frame, stddev, min_chunk_size));
    }

    return 0;
}
