/* CVLib utils functions.
 * @file
 * @date 2018-09-10
 * @author Max Kimlyk
 */

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <opencv2/opencv.hpp>

namespace cvlib_utils
{
template<class IntegralType>
IntegralType sum_in_rect_by_integral(const cv::Mat& integral, const cv::Rect& rect)
{
    cv::Point a(rect.x, rect.y);
    cv::Point b(rect.x + rect.width, rect.y);
    cv::Point c(rect.x + rect.width, rect.y + rect.height);
    cv::Point d(rect.x, rect.y + rect.height);

    return integral.at<IntegralType>(c) - integral.at<IntegralType>(b) - integral.at<IntegralType>(d) + integral.at<IntegralType>(a);
}

void mean_stddev_by_integral(const cv::Mat& integral, const cv::Mat& integral_sq, const cv::Rect& rect, double& mean, double& std);
void mean_disp_by_integral(const cv::Mat& integral, const cv::Mat& integral_sq, const cv::Rect& rect, double& mean, double& disp);
} // namespace cvlib_utils

#endif
