/* Split and merge segmentation algorithm testing.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <catch2/catch.hpp>

#include "cvlib.hpp"

using namespace cvlib;

TEST_CASE("empty image", "[cvlib_merge]")
{
    cv::Mat image;
    auto res = cvlib_merge(image, 1);
    REQUIRE(res.empty());
}


TEST_CASE("constant image", "[cvlib_merge]")
{
    const cv::Mat image(100, 100, CV_8UC1, cv::Scalar{ 15 });

    const auto res = cvlib_merge(image, 1);
    REQUIRE(image.size() == res.size());
    REQUIRE(image.type() == res.type());
    REQUIRE(cv::Scalar(15) == cv::mean(res));
}


TEST_CASE("simple regions", "[cvlib_merge]")
{
    SECTION("2x2")
    {
        const cv::Mat reference = (cv::Mat_<char>(2, 2) <<
            2, 2,
            2, 2);
        cv::Mat image = (cv::Mat_<char>(2, 2) <<
            0, 1,
            2, 3);

        auto res = cvlib_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = cvlib_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) <<
            4, 4, 4,
            4, 4, 4,
            4, 4, 4);
        cv::Mat image = (cv::Mat_<char>(3,3) <<
            0, 1, 2,
            3, 4, 5,
            6, 7, 8);
        cv::Mat res = cvlib_merge(image, 10);

        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = cvlib_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));

    }
}

TEST_CASE("compex regions", "[split_and_merge]")
{
    SECTION("2x2")
    {
        const cv::Mat reference = (cv::Mat_<char>(2, 2) <<
            1, 41,
            88, 41);
        cv::Mat image = (cv::Mat_<char>(2, 2) <<
            1, 44,
            88, 38);

        auto res = cvlib_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = cvlib_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));

    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) <<
            2, 18, 18,
            2, 88, 5,
            2, 79, 79);
        cv::Mat image = (cv::Mat_<char>(3, 3) << 
            0, 14, 22,
            2, 88, 5,
            6, 76, 82);
        cv::Mat res = cvlib_merge(image, 10);

        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = cvlib_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("4x4")
    {
        const cv::Mat reference = (cv::Mat_<char>(4, 4) <<
            8, 800, 55, 5,
            4, 4,   35, 22,
            14, 88, 12, 88,
            68, 68, 12, 88);
        cv::Mat image = (cv::Mat_<char>(4, 4) <<
            8, 800, 55, 5,
            3, 5, 35, 22,
            14, 88, 11, 88,
            67, 69, 14, 88);
        cv::Mat res = cvlib_merge(image, 10);

        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = cvlib_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }
}
