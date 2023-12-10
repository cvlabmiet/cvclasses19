/* Demo application for Computer Vision Library.
 * @file
 * @date 2023-12-10
 * @author Yaroslav Murenkov
 */

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

#include "utils.hpp"

auto histogram = [](cv::Mat& src, cv::Mat& output) {
    const cv::Scalar background_color(0, 0, 0);
    const auto [height, width] = std::make_pair(480, 640);
    const auto histogram_size = 256; // the number of "bins"
    const float range[] = {0, 256}; // upper bound is exclusive, meaning 0-255
    const float* ranges = {range};
    const bool uniform = true;
    const bool accumulate = false;
    cv::Mat mask;

    const int margin = 3;
    const int min_y = margin;
    const int max_y = height - margin;
    const int thickness = 1;
    const int line_type = cv::LINE_AA;
    const float bin_width = static_cast<float>(width) / static_cast<float>(histogram_size);
    cv::Mat dst(height, width, CV_8UC3, background_color);
    const cv::Scalar color = cv::Scalar(255, 255, 255);

    cv::Mat histogram;
    cv::calcHist(&src, 1, 0, mask, histogram, 1, &histogram_size, &ranges, uniform, accumulate);
    cv::normalize(histogram, histogram, 0, dst.rows, cv::NORM_MINMAX);

    for (int i = 1; i < histogram_size; ++i)
    {
        const int x1 = std::round(bin_width * (i - 1));
        const int x2 = std::round(bin_width * (i - 0));

        const int y1 = std::clamp(height - static_cast<int>(std::round(histogram.at<float>(i - 1))), min_y, max_y);
        const int y2 = std::clamp(height - static_cast<int>(std::round(histogram.at<float>(i - 0))), min_y, max_y);

        cv::line(dst, cv::Point(x1, y1), cv::Point(x2, y2), color, thickness, line_type);
    }
    dst.copyTo(output);
};

int demo_feature_descriptor(int argc, char* argv[])
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

    const auto main_wnd = "orig";
    const auto demo_wnd = "demo";

    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);

    cv::Mat frame;
    cv::Mat frame_blur;
    auto detector_a = cvlib::corner_detector_fast::create();
    auto detector_b = cv::ORB::create();
    std::vector<cv::KeyPoint> corners;

    utils::fps_counter fps;
    int pressed_key = 0;
    const auto ESC_KEY_CODE = 27;
    const auto SPACE_KEY_CODE = 32;
    while (pressed_key != ESC_KEY_CODE)
    {
        cap >> frame;
        cv::imshow(main_wnd, frame);

        cv::blur(frame, frame_blur, cv::Size(9, 9));
        detector_a->detect(frame_blur, corners);
        cv::drawKeypoints(frame, corners, frame, cv::Scalar(0, 0, 255));

        utils::put_fps_text(frame, fps);
        std::stringstream ss;
        ss << "Corners: " << corners.size();
        cv::putText(frame, ss.str(), cv::Point(20, 40), cv::FONT_HERSHEY_DUPLEX, 1.0, CV_RGB(0, 255, 0));
        cv::imshow(demo_wnd, frame);

        pressed_key = cv::waitKey(30);
        if (pressed_key == SPACE_KEY_CODE) // space
        {
            const auto hist_wnd = "hist";
            cv::namedWindow(hist_wnd);

            cv::Mat descriptors_cvlib;
            cv::Mat descriptors_cv;
            detector_a->compute(frame, corners, descriptors_cvlib);
            detector_b->compute(frame, corners, descriptors_cv);
            descriptors_cv.resize(descriptors_cv.size().height);

            cv::Mat hamming_dist = cv::Mat(descriptors_cvlib.size(), descriptors_cvlib.type());
            cv::bitwise_xor(descriptors_cvlib, descriptors_cv, hamming_dist);

            cv::Mat hamming_hist;
            histogram(hamming_dist, hamming_hist);
            cv::imshow(hist_wnd, hamming_hist);

            std::cout << "Dump descriptors complete!\n";
        }

        std::cout << "Feature points: " << corners.size() << "\r";
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
