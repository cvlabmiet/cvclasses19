/* Split and merge segmentation algorithm testing.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <catch2/catch.hpp>
#include <iostream>
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
        const cv::Mat reference = (cv::Mat_<char>(2, 2) << 2, 2, 2, 2);
        cv::Mat image = (cv::Mat_<char>(2, 2) << 0, 1, 2, 3);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));
        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));

    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) << 4, 4, 4, 4, 4, 4, 4, 4, 4);
        cv::Mat image = (cv::Mat_<char>(3, 3) << 0, 1, 2, 3, 4, 5, 6, 7, 8);
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
        const cv::Mat reference = (cv::Mat_<char>(2, 2) << 2, 2, 37, 37);
        cv::Mat image = (cv::Mat_<char>(2, 2) << 1, 3, 42, 32);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));
        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) << 2, 2, 2, 70, 15, 15, 70, 15, 15);
        cv::Mat image = (cv::Mat_<char>(3, 3) << 1, 2, 3, 69, 15, 16, 71, 14, 15);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));
        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("4x4")
    {
        const cv::Mat reference = (cv::Mat_<char>(4, 4) << 3, 3, 3, 3, 
                                                           3, 3, 3, 3, 
                                                           20, 20, 20, 20, 
                                                           40, 40, 40, 40);
        cv::Mat image = (cv::Mat_<char>(4, 4) << 1, 2, 3, 2,
                                                 4, 5, 3, 4, 
                                                 21, 19, 22, 18,
                                                 41, 39, 40, 40);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));
        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("4x4 Extra")
    {
        const cv::Mat reference = (cv::Mat_<char>(4, 4) << 0, 0, 100, 100, 
                                                           0, 100, 100, 100, 
                                                           0, 0, 0, 0, 
                                                           0, 0, 0, 0);

        cv::Mat image = (cv::Mat_<char>(4, 4) << 0, 0, 102, 100, 
                                                 0, 100, 102, 100, 
                                                 0, 0, 0, 0, 
                                                 0, 0, 0, 0);
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        std::cout << res << std::endl;
        REQUIRE(0 == cv::countNonZero(reference - res));
        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }
}