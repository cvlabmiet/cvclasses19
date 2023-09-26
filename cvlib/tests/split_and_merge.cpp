/* Split and merge segmentation algorithm testing.
 * @file
 * @date 2023-11-14
 * @author Yaroslav Murenkov
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
        // clang-format off
        const cv::Mat image = (cv::Mat_<char>(2, 2) <<
                0, 1,
                2, 3
        );
        const cv::Mat reference = (cv::Mat_<char>(2, 2) <<
                2, 2,
                2, 2
        );
        // clang-format on
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("3x3")
    {
        // clang-format off
        const cv::Mat image = (cv::Mat_<char>(3, 3) <<
                0, 1, 2,
                3, 4, 5,
                6, 7, 8
        );
        const cv::Mat reference = (cv::Mat_<char>(3, 3) <<
                4, 4, 4,
                4, 4, 4,
                4, 4, 4
        );
        // clang-format on
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
        // clang-format off
        const cv::Mat image = (cv::Mat_<char>(2, 2) <<
                 5,  7,
                40, 41
        );
        const cv::Mat reference = (cv::Mat_<char>(2, 2) <<
                 6,  6,
                40, 41
        );
        // clang-format on
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(6 == res.at<char>(0, 0));
        REQUIRE(6 == res.at<char>(0, 1));
        REQUIRE(40 == res.at<char>(0, 2));
        REQUIRE(40 == res.at<char>(1, 0));
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("3x3")
    {
        // clang-format off
        const cv::Mat image = (cv::Mat_<char>(3, 3) <<
                40, 44, 48,
                44,  4,  6,
                44,  7,  8
        );
        const cv::Mat reference = (cv::Mat_<char>(3, 3) <<
                44, 44, 44,
                44,  6,  6,
                44,  6,  6
        );
        // clang-format on
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }

    SECTION("4x4")
    {
        // clang-format off
        const cv::Mat image = (cv::Mat_<char>(4, 4) <<
                40, 43, 0, 3,
                41, 42, 6, 1,
                98, 92, 4, 2,
                96, 94, 5, 7
        );
        const cv::Mat reference = (cv::Mat_<char>(4, 4) <<
                42, 42, 3, 3,
                42, 42, 3, 3,
                95, 95, 3, 3,
                95, 95, 3, 3
        );
        // clang-format on
        auto res = split_and_merge(image, 10);
        REQUIRE(image.size() == res.size());
        REQUIRE(image.type() == res.type());
        REQUIRE(0 == cv::countNonZero(reference - res));

        res = split_and_merge(image, 1);
        REQUIRE(0 == cv::countNonZero(image - res));
    }
}
