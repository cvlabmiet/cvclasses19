/* Split and merge segmentation algorithm testing.
 * @file
 * @date 2018-09-10
 * @author Max Kimlyk
 */

#include <catch2/catch.hpp>

#include "utils.hpp"

using namespace cvlib_utils;

TEST_CASE("integral image", "[test_utils]")
{
    double EPS = 0.0001;

    int width = 1000;
    int height = width;

    cv::Mat image = cv::Mat(height, width, CV_8UC1);
    cv::randu(image, cv::Scalar(0), cv::Scalar(255));

    cv::Mat integral;
    cv::Mat integral_sq;
    cv::integral(image, integral, integral_sq);

    cv::Rect rect(width / 2, height / 2, width / 2, height / 2);

    cv::Scalar mean = 0;
    cv::Scalar std = 0;
    meanStdDev(image(rect), mean, std);

    double integral_mean = 0;
    double integral_std = 0;
    mean_stddev_by_integral(integral, integral_sq, rect, integral_mean, integral_std);

    CHECK(std::abs(integral_mean - mean(0)) < EPS);
    CHECK(std::abs(integral_std - std(0)) < EPS);
}
