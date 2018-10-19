/* FAST corner detector algorithm implementation.
 * @file
 * @date 2018-10-16
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>

namespace cvlib
{
// static
cv::Ptr<corner_detector_fast> corner_detector_fast::create()
{
    return cv::makePtr<corner_detector_fast>();
}

void corner_detector_fast::detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray /*mask = cv::noArray()*/)
{
    keypoints.clear();
    // \todo implement FAST with minimal LOCs(lines of code), but keep code readable.

    cv::Size image_size = image.size();
    cv::Mat imag = image.getMat();

    std::vector<int> circle_points(16);
    std::vector<int> main_stream(4);
    std::vector<int> target_plus;
    std::vector<int> target_minus;
    std::vector<int> target;

    int i_c;
    int i_plus_thresh;
    int i_minus_thresh;

    // std::cout << m_threshold << std::endl;
    for (auto i = 3; i < image_size.height - 3; i++)
    {
        for (auto j = 3; j < image_size.width - 3; j++)
        {
            i_c = imag.at<unsigned char>(i, j);
            i_plus_thresh = i_c + m_threshold;
            i_minus_thresh = i_c - m_threshold;

            circle_points.push_back(imag.at<unsigned char>(i - 3, j)); // 1
            circle_points.push_back(imag.at<unsigned char>(i - 3, j + 1)); // 2
            circle_points.push_back(imag.at<unsigned char>(i - 2, j + 2)); // 3
            circle_points.push_back(imag.at<unsigned char>(i - 1, j + 3)); // 4
            circle_points.push_back(imag.at<unsigned char>(i, j + 3)); // 5
            circle_points.push_back(imag.at<unsigned char>(i + 1, j + 3)); // 6
            circle_points.push_back(imag.at<unsigned char>(i + 2, j + 2)); // 7
            circle_points.push_back(imag.at<unsigned char>(i + 3, j + 1)); // 8
            circle_points.push_back(imag.at<unsigned char>(i + 3, j)); // 9
            circle_points.push_back(imag.at<unsigned char>(i + 3, j - 1)); // 10
            circle_points.push_back(imag.at<unsigned char>(i + 2, j - 2)); // 11
            circle_points.push_back(imag.at<unsigned char>(i + 1, j - 3)); // 12
            circle_points.push_back(imag.at<unsigned char>(i, j - 3)); // 13
            circle_points.push_back(imag.at<unsigned char>(i - 1, j - 3)); // 14
            circle_points.push_back(imag.at<unsigned char>(i - 2, j - 2)); // 15
            circle_points.push_back(imag.at<unsigned char>(i - 3, j - 1)); // 16

            // проверяем точки 1, 5, 9, 13
            int greath = 0;
            int less = 0;
            for (auto l = circle_points.begin(); l != circle_points.end(); l += 4)
            {
                greath = *l > i_plus_thresh ? ++greath : greath;
                less = *l < i_minus_thresh ? ++less : less;
            }

            if (greath >= 3 || less >= 3)
            {
                // std::copy_if(circle_points.begin(), circle_points.end(), std::back_inserter(target), [=](int n) { return n < i_minus_thresh; });

                // количество темных точек
                int minus = std::count_if(circle_points.begin(), circle_points.end(), [=](int n) { return n < i_minus_thresh; });

                // если их больше либо равно 12, то будем проверять идут ли они подряд
                if (minus >= 12)
                {
                    keypoints.push_back(cv::KeyPoint(cv::Point2f(double(j), double(i)), 10, float(0)));
                }

                // target.clear();
                // std::copy_if(circle_points.begin(), circle_points.end(), std::back_inserter(target), [=](int n) { return n > i_plus_thresh; });
                // количество светлых точек
                int plus = std::count_if(circle_points.begin(), circle_points.end(), [=](int n) { return n > i_plus_thresh; });
                // >= 12, то проверим идут ли они подряд
                if (plus >= 12)
                {
                    keypoints.push_back(cv::KeyPoint(cv::Point2f(double(j), double(i)), float(0)));
                }
            }

            circle_points.clear();
        }
    }
}
} // namespace cvlib
