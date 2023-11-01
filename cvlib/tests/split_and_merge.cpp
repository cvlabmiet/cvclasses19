/* Split and merge segmentation algorithm testing.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <catch2/catch.hpp>

#include "cvlib.hpp"

using namespace cvlib;

TEST_CASE("constant image", "[split_and_merge]")
{
    const cv::Mat image(100, 100, CV_8UC1, cv::Scalar{15});

    const auto res = split_and_merge(image, 1);
    REQUIRE(image.size() == res.size());
    REQUIRE(image.type() == res.type());
    REQUIRE(cv::Scalar(15) == cv::mean(res));
}

TEST_CASE("simple regions", "[split_and_merge]")
{
    SECTION("2x2")
    {
        const cv::Mat reference = (cv::Mat_<char>(2, 2) << 2, 2, 
                                                           2, 2);
        cv::Mat image = (cv::Mat_<char>(2, 2) << 0, 1, 
                                                 2, 3);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) << 4, 4, 4,
                                                           4, 4, 4, 
                                                           4, 4, 4);
        cv::Mat image = (cv::Mat_<char>(3, 3) << 0, 1, 2, 
                                                 3, 4, 5, 
                                                 6, 7, 8);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }
}

TEST_CASE("compex regions", "[split_and_merge]")
{
    SECTION("2x2")
    {
        const cv::Mat reference = (cv::Mat_<char>(2, 2) << 4, 4,
                                                           4, 4);
        cv::Mat image = (cv::Mat_<char>(2, 2) << 5, 3, 
                                                 2, 6);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) << 4, 6, 6,
                                                           4, 28, 77,
                                                           4, 52, 52);

        cv::Mat image = (cv::Mat_<char>(3, 3) << 8, 4, 6,
                                                 3, 100, 7,
                                                 3, 3, 100);

        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("4x4")
    {
        const cv::Mat reference = (cv::Mat_<char>(4, 4) << 54, 54, 5, 5,
                                                           54, 54, 5, 5,
                                                           30, 30, 30, 30,
                                                           30, 30, 30, 30);
        cv::Mat image = (cv::Mat_<char>(4, 4) << 4, 6, 3, 7,
                                                 8, 2, 5, 5,
                                                 104, 102, 0, 10,
                                                 100, 108, 7, 3);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }
}
