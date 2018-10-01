/* Split and merge segmentation algorithm testing.
 * @file
 * @date 2018-09-17
 * @author Vladislav Dimakov
 */

#include <catch2/catch.hpp>

#include "cvlib.hpp"

using namespace cvlib;

TEST_CASE("empty image select_texture", "[select_texture]")
{
    const cv::Mat image;
    const cv::Rect roi(0, 0, 10, 10);
    const cv::Mat res = select_texture(image, roi, 50);
    REQUIRE(res.empty());
}

TEST_CASE("constant image select_texture", "[select_texture]")
{
    const cv::Mat image(100, 100, CV_8UC1, cv::Scalar(255));
    const cv::Rect roi(0, 0, 50, 50);
    const cv::Mat res = select_texture(image, roi, 50);

    REQUIRE(image.size() == res.size());
    REQUIRE(image.type() == res.type());
    REQUIRE(cv::countNonZero(image(roi) - res(roi)) == 0);
}

TEST_CASE("little ROI select_texture", "[select_texture]")
{
    const cv::Mat image(100, 100, CV_8UC1, cv::Scalar(255));
    const cv::Mat ref(image.size(), CV_8UC1, cv::Scalar(0));
    const cv::Rect roi(0, 0, 0, 1);
    const cv::Mat res = select_texture(image, roi, 50);

    REQUIRE(cv::countNonZero(res - ref) == 0);
}

TEST_CASE("big ROI select_texture", "[select_texture]")
{
    const cv::Mat image(100, 100, CV_8UC1, cv::Scalar(255));
    const cv::Mat ref(image.size(), CV_8UC1, cv::Scalar(255));
    const cv::Rect roi(0, 0, 100, 100);
    const cv::Mat res = select_texture(image, roi, 50);

    REQUIRE(cv::countNonZero(res - ref) == 0);
}
