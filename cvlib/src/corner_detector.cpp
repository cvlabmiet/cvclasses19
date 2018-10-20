/* FAST corner detector algorithm implementation.
 * @file
 * @date 2018-10-16
 * @author Dmitrij Vukalov
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
    m_num_point = 0;
    cv::Size image_size = image.size();
    cv::Mat imag = image.getMat();
    //                  1    2   3   4  5  6  7  8  9  10 11 12 13 14  15  16
    int offset_i[16] = {-3, -3, -2, -1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2, -3};
    int offset_j[16] = {0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2, -1};

    std::vector<int> circle_points(16);
    int i_c, i_plus_thresh, i_minus_thresh;

    for (auto i = 3; i < image_size.height - 3; i++)
    {
        for (auto j = 3; j < image_size.width - 3; j++)
        {
            i_c = imag.at<unsigned char>(i, j);
            i_plus_thresh = i_c + m_threshold;
            i_minus_thresh = i_c - m_threshold;

            for (auto k = 0; k < 16; k++)
                circle_points.push_back(imag.at<unsigned char>(i + offset_i[k], j + offset_j[k]));

            int light = 0, dark = 0; // проверяем точки 1, 5, 9, 13
            for (auto l = circle_points.begin(); l != circle_points.end(); l += 4)
            {
                light = *l > i_plus_thresh ? ++light : light;
                dark = *l < i_minus_thresh ? ++dark : dark;
            }

            if (light > 2 || dark > 2)
            {
                int num_dark_points = std::count_if(circle_points.begin(), circle_points.end(), [=](int n) { return (n < i_minus_thresh); });
                if (num_dark_points > 11)
                {
                    m_num_point++;
                    keypoints.push_back(cv::KeyPoint(cv::Point2f(double(j), double(i)), 10, float(0)));
                }
                else
                {
                    int num_ligth_points = std::count_if(circle_points.begin(), circle_points.end(), [=](int n) { return n > i_plus_thresh; });
                    if (num_ligth_points > 11)
                    {
                        m_num_point++;
                        keypoints.push_back(cv::KeyPoint(cv::Point2f(double(j), double(i)), 10, float(0)));
                    }
                }
            }

            circle_points.clear();
        }
    }
}
} // namespace cvlib
