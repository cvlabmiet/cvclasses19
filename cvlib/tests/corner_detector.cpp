/* FAST corner detector algorithm testing.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <catch2/catch.hpp>

#include "cvlib.hpp"

#include <iostream>

using namespace cvlib;

TEST_CASE("simple check", "[corner_detector_fast]")
{
    auto fast = corner_detector_fast::create();
    cv::Mat image(10, 10, CV_8UC1, cv::Scalar::all(0));
    SECTION("empty image")
    {
        std::vector<cv::KeyPoint> out;
        fast->detect(image, out);
        REQUIRE(out.empty());
    }

    cv::Mat small_image = (cv::Mat_<char>(7, 7) << 0, 0, 0,  0,  0,  0,  0,
												   0, 0, 0,  0,  0,  0,  0,
												   0, 0, 0,  0,  0,  0,  0,
												   0, 0, 0, 99, 99, 99, 98,
												   0, 0, 0, 99, 99, 99, 97,
												   0, 0, 0,  0, 99, 96, 99,
												   0, 0, 0,  0,  0, 99, 99);
	SECTION("one keypoint")
    {
        std::vector<cv::KeyPoint> out;
        fast->detect(small_image, out);
        REQUIRE(out.size() == 1);
        REQUIRE(out.at(0).pt.x == 3);
        REQUIRE(out.at(0).pt.y == 3);
    }

	small_image = (cv::Mat_<char>(7, 7) << 99, 99, 0, 0, 0, 0, 0,
										   99, 99, 99, 0, 0, 0, 0,
										   99, 99, 99, 0, 0, 0, 0,
										   0, 99, 99, 99, 0, 0, 0,
										   0, 0, 0, 0, 0, 0, 0,
										   0, 0, 0, 0, 0, 0, 0,
										   0, 0, 0, 0, 0, 0, 0);
	SECTION("another one keypoint")
    {
        std::vector<cv::KeyPoint> out;
        fast->detect(small_image, out);
        REQUIRE(out.size() == 1);
        REQUIRE(out.at(0).pt.x == 3);
        REQUIRE(out.at(0).pt.y == 3);
    }
	
	cv::Mat med_image = (cv::Mat_<char>(8, 8) << 0, 0, 0, 99, 99, 99, 99, 99,
												 0, 0, 0, 99, 99, 99, 0, 0,
												 0, 0, 0, 99, 99, 99, 0, 0,
												 0, 0, 0, 99, 99, 99, 0, 0,
												 0, 0, 0, 0, 0, 0, 0, 0,
												 0, 0, 0, 0, 0, 0, 0, 0,
												 0, 0, 0, 0, 0, 0, 0, 0,
												 0, 0, 0, 0, 0, 0, 0, 0);
	SECTION("two keypoints")
    {
        std::vector<cv::KeyPoint> out;
        fast->detect(med_image, out);
        REQUIRE(out.size() == 2);
        REQUIRE(out.at(0).pt.x == 3);
        REQUIRE(out.at(0).pt.y == 3);
        REQUIRE(out.at(1).pt.x == 4);
        REQUIRE(out.at(1).pt.y == 3);
    }	
	
	med_image = (cv::Mat_<char>(8, 8) << 0, 0, 0, 0, 0, 0, 0, 99,
										 0, 0, 0, 0, 0, 0, 0, 99,
										 0, 0, 0, 0, 99, 99, 99, 99,
										 0, 0, 0, 0, 99, 99, 99, 99,
										 0, 0, 0, 0, 99, 99, 99, 0,
										 0, 0, 0, 0, 0, 0, 0, 0,
										 0, 0, 0, 0, 0, 0, 0, 0,
										 0, 0, 0, 0, 0, 0, 0, 0);
	SECTION("another two keypoints")
    {
        std::vector<cv::KeyPoint> out;
        fast->detect(med_image, out);
        REQUIRE(out.size() == 2);
        REQUIRE(out.at(0).pt.x == 4);
        REQUIRE(out.at(0).pt.y == 3);
        REQUIRE(out.at(1).pt.x == 4);
        REQUIRE(out.at(1).pt.y == 4);
    }
	
	med_image = (cv::Mat_<char>(8, 8) << 0, 0, 0, 0, 0, 0, 0, 0,
										 0, 0, 0, 0, 0, 0, 0, 0,
										 0, 0, 0, 99, 99, 0, 0, 0,
										 0, 0, 99, 99, 99, 99, 0, 0,
										 0, 0, 99, 99, 99, 99, 0, 0,
										 0, 0, 0, 99, 99, 0, 0, 0,
										 0, 0, 0, 0, 0, 0, 0, 0,
										 0, 0, 0, 0, 0, 0, 0, 0);
	SECTION("three keypoints")
    {
        std::vector<cv::KeyPoint> out;
        fast->detect(med_image, out);
        REQUIRE(out.size() == 4);
        REQUIRE(out.at(0).pt.x == 3);
        REQUIRE(out.at(0).pt.y == 3);
        REQUIRE(out.at(1).pt.x == 3);
        REQUIRE(out.at(1).pt.y == 4);
        REQUIRE(out.at(2).pt.x == 4);
        REQUIRE(out.at(2).pt.y == 3);
        REQUIRE(out.at(3).pt.x == 4);
        REQUIRE(out.at(3).pt.y == 4);
    }
}
