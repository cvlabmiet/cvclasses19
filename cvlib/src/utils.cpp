/* CVLib utils functions.
 * @file
 * @date 2018-09-10
 * @author Max Kimlyk
 */

#include "utils.hpp"

#include <cmath>

namespace cvlib_utils
{
void mean_disp_by_integral(const cv::Mat& integral, const cv::Mat& integral_sq, const cv::Rect& rect, double& mean, double& disp)
{
    double area = (double)(rect.area());

    if (area == 0.0)
    {
        mean = 0.0;
        disp = 0.0;
        return;
    }

    cv::Point a(rect.x, rect.y);
    cv::Point b(rect.x + rect.width, rect.y);
    cv::Point c(rect.x + rect.width, rect.y + rect.height);
    cv::Point d(rect.x, rect.y + rect.height);

    int sum = integral.at<int>(c) - integral.at<int>(b) - integral.at<int>(d) + integral.at<int>(a);
    double sum_sq = integral_sq.at<double>(c) - integral_sq.at<double>(b) - integral_sq.at<double>(d) + integral_sq.at<double>(a);

    mean = (double)(sum) / area;
    disp = sum_sq / area - mean * mean;
}

void mean_stddev_by_integral(const cv::Mat& integral, const cv::Mat& integral_sq, const cv::Rect& rect, double& mean, double& std)
{
    double disp = 0.0;
    mean_disp_by_integral(integral, integral_sq, rect, mean, disp);
    std = std::sqrt(disp);
}
} // namespace cvlib_utils
