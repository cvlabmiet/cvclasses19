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
    cv::Mat image, splitImage, mergeImage;
    split_and_merge(image, splitImage, mergeImage, 1, 0, 0, 1);
    REQUIRE(mergeImage.empty());
}

TEST_CASE("constant image", "[split_and_merge]")
{
    const cv::Mat image(100, 100, CV_8UC1, cv::Scalar{15});
    cv::Mat splitImage, mergeImage;
    split_and_merge(image, splitImage, mergeImage, 1, 0, 0, 1);

    REQUIRE(image.size() == mergeImage.size());
    REQUIRE(image.type() == mergeImage.type());
    REQUIRE(cv::Scalar(15) == cv::mean(mergeImage));
}

TEST_CASE("simple regions", "[split_and_merge]")
{
    SECTION("2x2")
    {
        const cv::Mat reference = (cv::Mat_<char>(2, 2) << 2, 2, 2, 2);
        cv::Mat splitImage, mergeImage, image = (cv::Mat_<char>(2, 2) << 0, 1, 2, 3);
        split_and_merge(image, splitImage, mergeImage, 2, 0, 0, 1);

        REQUIRE(image.size() == mergeImage.size());
        REQUIRE(image.type() == mergeImage.type());
        REQUIRE(0 == cv::countNonZero(reference - mergeImage));

        split_and_merge(image, splitImage, mergeImage, 1, 0, 0, 1);
        REQUIRE(0 == cv::countNonZero(image - mergeImage));
    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) << 4, 4, 4, 4, 4, 4, 4, 4, 4);
        cv::Mat splitImage, mergeImage, image = (cv::Mat_<char>(3, 3) << 0, 1, 2, 3, 4, 5, 6, 7, 8);
        split_and_merge(image, splitImage, mergeImage, 3, 0, 0, 1);
        REQUIRE(0 == cv::countNonZero(reference - mergeImage));
    }
}

TEST_CASE("complex regions", "[split_and_merge]")
{
    SECTION("2x2")
    {
        const cv::Mat reference = (cv::Mat_<char>(2, 2) << 0, 100, 100, 100);
        cv::Mat splitImage, mergeImage, image = (cv::Mat_<char>(2, 2) << 0, 99, 100, 101);
        split_and_merge(image, splitImage, mergeImage, 40, 0, 1, 1);
        REQUIRE(0 == cv::countNonZero(reference - mergeImage));

        split_and_merge(image, splitImage, mergeImage, 40, 0, 0, 1);
        REQUIRE(0 == cv::countNonZero(image - mergeImage));
    }

    SECTION("3x3")
    {
        const cv::Mat reference = (cv::Mat_<char>(3, 3) << 0, 0, 0, 0, 100, 100, 0, 100, 100);
        cv::Mat splitImage, mergeImage, image = (cv::Mat_<char>(3, 3) << 0, 0, 0, 0, 98, 99, 0, 100, 101);
        split_and_merge(image, splitImage, mergeImage, 40, 0, 1, 1);
        REQUIRE(0 == cv::countNonZero(reference - mergeImage));
    }

    SECTION("4x4")
    {
        const cv::Mat reference = (cv::Mat_<char>(4, 4) << 0, 0, 0, 0, 0, 0, 100, 100, 50, 50, 100, 100, 50, 50, 100, 100);
        cv::Mat splitImage, mergeImage, image = (cv::Mat_<char>(4, 4) << 0, 0, 0, 0, 0, 0, 97, 98, 48, 49, 99, 100, 50, 51, 101, 102);
        split_and_merge(image, splitImage, mergeImage, 40, 0, 1, 2);
        REQUIRE(0 == cv::countNonZero(reference - mergeImage));
    }
}
