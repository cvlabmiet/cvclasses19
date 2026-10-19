/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

int demo_motion_segmentation(int argc, char* argv[])
{

    cv::VideoCapture cap(0, cv::CAP_V4L2);
    if (!cap.isOpened()) {
       std::cerr << "Cannot open camera" << std::endl;
       return -1;
    }
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    auto mseg = cv::makePtr<cvlib::motion_segmentation>(/*alpha=*/0.001);
    const auto main_wnd = "orig";
    const auto demo_wnd = "demo";

    int threshold = 25;                                    // 0..255
    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);
    cv::createTrackbar("th", demo_wnd, &threshold, 100);

    cv::Mat frame;
    cv::Mat frame_blur;
    cv::Mat frame_mseg;
    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;
        if (frame.empty())
            break;

        cv::imshow(main_wnd, frame);

        cv::GaussianBlur(frame, frame_blur, cv::Size(7, 7), 0);

        mseg->setDiffThreshold(static_cast<double>(threshold));
        mseg->apply(frame_blur, frame_mseg);

        if (!frame_mseg.empty())
            cv::imshow(demo_wnd, frame_mseg);
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}