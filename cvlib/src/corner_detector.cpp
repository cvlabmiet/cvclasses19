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

    std::vector<int> etalon_1(12);
    std::vector<int> etalon_2(12);
    std::fill(etalon_1.begin(), etalon_1.end(), 1);
    std::fill(etalon_2.begin(), etalon_2.end(), 2);

    int i_c, i_plus_thresh, i_minus_thresh;

    for (auto i = 3; i < image_size.height - 3; i++)
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
                copyVector();
                std::transform(cyclic_buffer.begin(), cyclic_buffer.end(), cyclic_buffer.begin(),
                               [=](int n) { return (n < i_minus_thresh) + 2 * (n > i_plus_thresh); });
                
                auto it_dark = std::search(cyclic_buffer.begin(), cyclic_buffer.end(), etalon_1.begin(), etalon_1.end());
                auto it_ligth = std::search(cyclic_buffer.begin(), cyclic_buffer.end(), etalon_2.begin(), etalon_2.end());
                
                if (it_dark != cyclic_buffer.end() || it_ligth != cyclic_buffer.end())
                {
                    m_num_point++;
                    keypoints.push_back(cv::KeyPoint(cv::Point2f(double(j), double(i)), 10, float(0)));
                }
                cyclic_buffer.clear();
            }
            circle_points.clear();
        }
}
} // namespace cvlib
