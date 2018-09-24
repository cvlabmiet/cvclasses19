/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-17
 * @author Vladislav Dimakov
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

    int stddev = 8;
	int minSquare = 25;
	int meanDeviation = 2;
	int scaleFactor = 20;

    cv::namedWindow(demo_wnd, 1);
    cv::createTrackbar("stddev", demo_wnd, &stddev, 128); // stddev[{0, 255}] = 127.5
	cv::createTrackbar("square", demo_wnd, &minSquare, 100);
	cv::createTrackbar("mean", demo_wnd, &meanDeviation, 10);
	cv::createTrackbar("scale", demo_wnd, &scaleFactor, 50);

    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;

        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        cv::imshow(origin_wnd, frame);
        cv::imshow(demo_wnd, cvlib::split_and_merge(frame_gray, stddev, minSquare, meanDeviation, scaleFactor));
    }

    return 0;
}
