/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <opencv2/opencv.hpp>

#include <cvlib.hpp>

int demo_split_and_merge(int argc, char* argv[])
{
    cv::VideoCapture cap(0, cv::CAP_V4L2);
    if (!cap.isOpened()) {
       std::cerr << "Cannot open camera" << std::endl;
       return -1;
    }
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
    //cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    //cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    //cv::VideoCapture cap(0);
    // if (!cap.isOpened())
    //    return -1;
    //for (int i = 0; i < 10; ++i) {
    //   cap.grab();
    //}

    cv::Mat frame;
    cv::Mat frame_gray;

    const auto origin_wnd = "origin";
    const auto demo_wnd = "demo";

    int stddev = 50;
    cv::namedWindow(demo_wnd, 1);
    // \todo choose reasonable max value
    cv::createTrackbar("stdev", demo_wnd, &stddev, 255);

    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;
	    if (frame.empty()) {
            std::cerr << "Warning: empty frame, skipping..." << std::endl;
            continue;
        }
        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        cv::imshow(origin_wnd, frame);
        cv::imshow(demo_wnd, cvlib::split_and_merge(frame_gray, stddev));
    }

    cv::destroyWindow(origin_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
