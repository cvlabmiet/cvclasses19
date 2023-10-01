/* Split and merge segmentation algorithm testing.
 * @file
 * @date 2023-10-01
 * @author Rururustm
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

TEST_CASE("complex regions", "[split_and_merge]")
{
    SECTION("2x2")
    {
        const cv::Mat reference = (cv::Mat_<char>(2, 2) << 10, 0, 10, 0);
        cv::Mat image = (cv::Mat_<char>(2, 2) << 10, 0, 10, 0);
        auto res = split_and_merge(image, 4);
        
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));
    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) << 10, 0, 10, 0, 0, 0, 10, 0, 10);
        cv::Mat image = (cv::Mat_<char>(3, 3) << 10, 0, 10, 0, 0, 0, 10, 0, 10);
        auto res = split_and_merge(image, 1);
        
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));
    }

    SECTION("4x4")
    {
        const cv::Mat reference = (cv::Mat_<int>(4, 4) << 3,   3,   3,   3, 
                                                          3,   3,   3,   3, 
                                                          200, 200, 200, 200, 
                                                          600, 600, 600, 600);
        cv::Mat image = (cv::Mat_<int>(4, 4) << 1,   2,   3,   2,
                                                4,   5,   3,   4, 
                                                201, 199, 210, 190,
                                                600, 600, 600, 600);
        auto res = split_and_merge(image, 10);
        
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));
    }
}
