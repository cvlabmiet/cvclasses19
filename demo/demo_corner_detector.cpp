/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

#include "utils.hpp"

int demo_corner_detector(int argc, char* argv[])
{
    cv::VideoCapture cap(0,cv::CAP_V4L2);
    if (!cap.isOpened())
        return -1;

    const auto main_wnd = "orig";
    const auto demo_wnd = "demo";

    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);
    int threshold = 5;
    cv::createTrackbar("th", demo_wnd, &threshold, 50);

    cv::Mat frame;
//    auto detector = cv::GFTTDetector::create(); // \todo use cvlib::corner_detector_fast
    auto detector = cvlib::corner_detector_fast();
    std::vector<cv::KeyPoint> corners;

    utils::fps_counter fps;
    while (cv::waitKey(30) != 27) // ESC
    {
        cap >> frame;
        cv::imshow(main_wnd, frame);

//        detector->detect(frame, corners);
        detector.thresh = threshold;
        detector.detect(frame,corners);
        cv::drawKeypoints(frame, corners, frame, cv::Scalar(0, 0, 255));
        utils::put_fps_text(frame, fps);
        utils::put_text(frame,std::to_string(corners.size()));
        // \todo add count of the detected corners at the top left corner of the image. Use green text color.
        cv::imshow(demo_wnd, frame);
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
