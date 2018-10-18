/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Dmitrij Vukalov
 */

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

#include "utils.hpp"

int demo_motion_segmentation(int argc, char* argv[])
{
    // const cv::String filename = "D:/ClangFormat/Video2.mp4";
    const int filename = 0;
    // cv::VideoCapture cap(0);
    cv::VideoCapture cap(filename);
    if (!cap.isOpened())
        return -1;

    // auto mseg = cv::createBackgroundSubtractorMOG2(); // \todo use cvlib::motion_segmentation
    auto mseg = cvlib::motion_segmentation();
    const auto main_wnd = "main";
    const auto demo_wnd = "demo";

    int threshold = 50;
    int alpha = 50;

    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);
    cv::createTrackbar("th", demo_wnd, &threshold, 255);
    cv::createTrackbar("alpha", demo_wnd, &alpha, 100);

    cv::Mat frame;
    cv::Mat frame_mseg;

    utils::fps_counter fps;
    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;

        if (frame.empty())
        {
            cap.release();
            // cap.open(0);
            cap.open(filename);
        }
        else
        {
            mseg.setThreshold(threshold);

            mseg.setAlpha(alpha);

            cv::imshow(main_wnd, frame);
            cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

            mseg.apply(frame, frame_mseg, 0);

            if (!frame_mseg.empty())
            {
                utils::put_fps_text(frame_mseg, fps);
                cv::imshow(demo_wnd, frame_mseg);
            }
        }
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
