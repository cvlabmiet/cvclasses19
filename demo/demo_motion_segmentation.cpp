/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

int demo_motion_segmentation(int argc, char* argv[])
{
    const int filename = 0;
    cv::VideoCapture cap(filename);
    if (!cap.isOpened())
        return -1;

    auto mseg = cvlib::motion_segmentation();
    const auto main_wnd = "orig";
    const auto demo_wnd = "demo";

    int threshold = 50;
    int alpha = 25;

    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);
    cv::createTrackbar("th", demo_wnd, &threshold, 255);
    cv::createTrackbar("alpha", demo_wnd, &alpha, 100);

    cv::Mat frame;
    cv::Mat frame_mseg;
    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;
        if (frame.empty())
        {
            cap.release();
            cap.open(filename);
        }
        else
        {
            mseg.setThreshold(threshold);
            mseg.setAlpha(alpha);
            cv::imshow(main_wnd, frame);
            cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
            mseg.apply(frame, frame_mseg,0);

            if (!frame_mseg.empty())
                cv::imshow(demo_wnd, frame_mseg);
        }
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
