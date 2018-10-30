/* FAST corner detector algorithm testing.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <catch2/catch.hpp>

#include "cvlib.hpp"

using namespace cvlib;

TEST_CASE("simple check", "[corner_detector_fast]")
{
	auto fast = corner_detector_fast::create();
	cv::Mat image(10, 10, CV_8UC1);
	SECTION("empty image")
	{
		std::vector<cv::KeyPoint> out;
		fast->detect(image, out);
		REQUIRE(out.empty());
	}

	SECTION("uniform image")
	{
		cv::Mat image(10, 10, CV_8UC1, cv::Scalar(127));
		std::vector<cv::KeyPoint> out;
		fast->detect(image, out);
		REQUIRE(out.empty());
	}
}

TEST_CASE("complex check", "[corner_detector_fast]")
{
	auto fast = corner_detector_fast::create();
	std::vector<cv::KeyPoint> out;

	SECTION("point")
	{
		cv::Mat image = (cv::Mat_<uint8_t>(9, 9) <<
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 3, 2, 0, 0, 0, 0, 0,
			0, 0, 0, 4, 3, 0, 0, 0, 0,
			0, 0, 3, 0, 80, 0, 0, 0, 0,
			0, 0, 5, 0, 0, 0, 3, 0, 0,
			0, 0, 0, 0, 0, 0, 5, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0);

		fast->detect(image, out);
		REQUIRE(1 == out.size());
	}

	SECTION("two points")
	{
		cv::Mat image = (cv::Mat_<uint8_t>(9, 9) <<
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 2, 2, 0, 0, 0, 0, 0,
			0, 0, 0, 5, 1, 0, 0, 0, 0,
			0, 0, 2, 123, 77, 9, 0, 0, 0,
			0, 0, 6, 0, 0, 0, 4, 0, 0,
			0, 0, 0, 0, 0, 0, 4, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0);

		fast->detect(image, out);
		REQUIRE(2 == out.size());
	}

	SECTION("round")
	{
		cv::Mat image = (cv::Mat_<uint8_t>(9, 9) <<
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 65, 0, 5, 0, 0, 0,
			0, 0, 65, 0, 0, 0, 10, 0, 0,
			0, 65, 0, 0, 0, 0, 0, 15, 0,
			0, 60, 0, 0, 88, 0, 0, 12, 0,
			0, 55, 0, 0, 0, 0, 0, 25, 0,
			0, 0, 50, 0, 0, 0, 30, 0, 0,
			0, 0, 0, 45, 32, 35, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0);

		fast->detect(image, out);
		REQUIRE(1 == out.size());
	}

	SECTION("corner")
	{
		cv::Mat image = (cv::Mat_<uint8_t>(9, 9) <<
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 44, 0, 0, 0,
			0, 0, 0, 0, 127, 3, 0, 0, 0,
			0, 0, 0, 34, 30, 56, 7, 0, 0,
			0, 0, 95, 100, 122, 13, 21, 0, 0,
			3, 90, 63, 2, 7, 55, 80, 80, 5,
			12, 54, 67, 198, 34, 165, 247, 9, 55);

		fast->detect(image, out);
		REQUIRE(1 == out.size());
	}
}
