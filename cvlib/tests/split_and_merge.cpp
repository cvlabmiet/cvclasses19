/* Split and merge segmentation algorithm testing.
 * @file
 * @date 2018-09-17
 * @author Vladislav Dimakov
 */

#include <catch2/catch.hpp>

#include "cvlib.hpp"

using namespace cvlib;

TEST_CASE("empty image", "[split_and_merge]")
{
    cv::Mat image;
    auto res = split_and_merge(image, 1, 0, 0, 1);
    REQUIRE(res.empty());
}

TEST_CASE("constant image", "[split_and_merge]")
{
    const cv::Mat image(100, 100, CV_8UC1, cv::Scalar{15});

    const auto res = split_and_merge(image, 1, 0, 0, 1);
    REQUIRE(image.size() == res.size());
    REQUIRE(image.type() == res.type());
    REQUIRE(cv::Scalar(15) == cv::mean(res));
}

TEST_CASE("simple regions", "[split_and_merge]")
{
    SECTION("2x2")
    {
        const cv::Mat reference = (cv::Mat_<char>(2, 2) << 2, 2, 2, 2);
        cv::Mat image = (cv::Mat_<char>(2, 2) << 0, 1, 2, 3);
        auto res = split_and_merge(image, 2, 0, 0, 1);

        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1, 0, 0, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) << 4, 4, 4, 4, 4, 4, 4, 4, 4);
        cv::Mat image = (cv::Mat_<char>(3, 3) << 0, 1, 2, 3, 4, 5, 6, 7, 8);
        auto res = split_and_merge(image, 3, 0, 0, 1);
        REQUIRE(0 == cv::countNonZero(reference - res));
    }
}

TEST_CASE("complex regions", "[split_and_merge]")
{
    SECTION("2x2")
    {
        const cv::Mat reference = (cv::Mat_<char>(2, 2) << 0, 100, 100, 100);
        cv::Mat image = (cv::Mat_<char>(2, 2) << 0, 99, 100, 101);
        auto res = split_and_merge(image, 40, 0, 1, 1);
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 40, 0, 0, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) << 0, 0, 0, 0, 100, 100, 0, 100, 100);
        cv::Mat image = (cv::Mat_<char>(3, 3) << 0, 0, 0, 0, 98, 99, 0, 100, 101);
        auto res = split_and_merge(image, 40, 0, 1, 1);
        REQUIRE(0 == cv::countNonZero(reference - res));
    }

    SECTION("4x4")
    {
        const cv::Mat reference = (cv::Mat_<char>(4, 4) << 0, 0, 0, 0, 0, 0, 100, 100, 50, 50, 100, 100, 50, 50, 100, 100);
        cv::Mat image = (cv::Mat_<char>(4, 4) << 0, 0, 0, 0, 0, 0, 97, 98, 48, 49, 99, 100, 50, 51, 101, 102);
        auto res = split_and_merge(image, 40, 0, 1, 2);
        REQUIRE(0 == cv::countNonZero(reference - res));
    }
}
