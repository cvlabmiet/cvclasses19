/* Split and merge segmentation algorithm testing.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <catch2/catch.hpp>

#include "cvlib.hpp"

using namespace cvlib;

TEST_CASE("empty image", "[split_and_merge]")
{
    cv::Mat image;
    auto res = split_and_merge(image, 1);
    REQUIRE(res.empty());
}

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
		const cv::Mat reference = (cv::Mat_<uint8_t>(3, 3) <<
			4, 4, 4,
			4, 4, 4,
			4, 4, 4);
		cv::Mat image = (cv::Mat_<uint8_t>(3, 3) <<
			0, 1, 2,
			3, 4, 5,
			6, 7, 8);
		auto res = split_and_merge(image, 20);
		REQUIRE(image.size() == res.size());
		REQUIRE(image.type() == res.type());
		REQUIRE(0 == cv::countNonZero(reference - res));
    }
}

TEST_CASE("compex regions", "[split_and_merge]")
{
    SECTION("2x2")
    {
		const cv::Mat reference = (cv::Mat_<uint8_t>(2, 2) << 1, 21, 21, 21);
		const cv::Mat image = (cv::Mat_<uint8_t>(2, 2) << 1, 20, 21, 22);
		auto res = split_and_merge(image, 5);
		REQUIRE(image.size() == res.size());
		REQUIRE(image.type() == res.type());
		REQUIRE(0 == cv::countNonZero(reference - res));
    }

    SECTION("3x3")
    {
		const cv::Mat reference = (cv::Mat_<uint8_t>(3, 3) <<
			2, 22, 22,
			2, 22, 22,
			2, 22, 22);
		const cv::Mat image = (cv::Mat_<uint8_t>(3, 3) <<
			1, 21, 21,
			2, 22, 22,
			3, 23, 23);
		auto res = split_and_merge(image, 5);
		REQUIRE(image.size() == res.size());
		REQUIRE(image.type() == res.type());
		REQUIRE(0 == cv::countNonZero(reference - res));
    }

    SECTION("4x4")
    {
		const cv::Mat image = (cv::Mat_<uint8_t>(4, 4) <<
			14, 10, 91, 91,
			13, 11, 12, 90,
			14, 12, 89, 89,
			13, 1, 1, 1);
		const cv::Mat reference = (cv::Mat_<uint8_t>(4, 4) <<
			12, 12, 90, 90,
			12, 12, 12, 90,
			12, 12, 90, 90,
			12, 1, 1, 1);
		auto res = split_and_merge(image, 3);
		REQUIRE(image.size() == res.size());
		REQUIRE(image.type() == res.type());
		REQUIRE(0 == cv::countNonZero(reference - res));
    }
}
