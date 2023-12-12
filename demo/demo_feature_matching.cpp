/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-11-25
 * @author Anonymous
 */

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

#include "utils.hpp"

int demo_feature_matching(int argc, char* argv[])
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

    const auto main_wnd = "orig";
    const auto demo_wnd = "demo";

    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);

    auto ratio = 25;

    auto detector = cvlib::corner_detector_fast::create();
    auto matcher = cvlib::descriptor_matcher(ratio); //\todo add trackbar to demo_wnd to tune ratio value

    /// \brief helper struct for tidy code
    struct img_features
    {
        cv::Mat img;
        std::vector<cv::KeyPoint> corners;
        cv::Mat descriptors;
    };

    img_features ref;
    img_features test;
    std::vector<std::vector<cv::DMatch>> pairs;

    cv::Mat main_frame;
    cv::Mat demo_frame;
    cv::createTrackbar("ratio", demo_wnd, &ratio, 255);
    utils::fps_counter fps;
    int pressed_key = 0;
    const auto ESC_KEY_CODE = 27;
    const auto SPACE_KEY_CODE = 32;
    while (pressed_key != ESC_KEY_CODE)
    {
        cap >> test.img;

        cv::blur(test.img, test.img, cv::Size(9, 9));
        detector->detect(test.img, test.corners);
        cv::drawKeypoints(test.img, test.corners, main_frame);
        cv::imshow(main_wnd, main_frame);

        pressed_key = cv::waitKey(30);
        if (pressed_key == SPACE_KEY_CODE) // space
        {
            ref.img = test.img.clone();
            detector->detectAndCompute(ref.img, cv::Mat(), ref.corners, ref.descriptors);
        }

        if (ref.corners.empty())
            continue;

        detector->compute(test.img, test.corners, test.descriptors);
        matcher.set_ratio(ratio);
        matcher.radiusMatch(test.descriptors, ref.descriptors, pairs, 100.0f);
        cv::drawMatches(test.img, test.corners, ref.img, ref.corners, pairs, demo_frame);

        utils::put_fps_text(demo_frame, fps);
        cv::imshow(demo_wnd, demo_frame);
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
