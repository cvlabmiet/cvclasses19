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
        image = (cv::Mat_<char>(10, 10) <<
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        std::vector<cv::KeyPoint> out;
        fast->setVarThreshold(20);
        fast->detect(image, out);
        REQUIRE(out.empty());
    }

    SECTION("one point")
    {

        image = (cv::Mat_<char>(10, 10) << 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 3, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

        std::vector<cv::KeyPoint> out;
        fast->setVarThreshold(2);
        fast->detect(image, out);
        REQUIRE(out.size() == 1);
    }

    SECTION("several point")
    {
        image = (cv::Mat_<char>(10, 10) <<
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 3, 0, 0, 4, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 7, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

        std::vector<cv::KeyPoint> out;
        fast->setVarThreshold(2);
        fast->detect(image, out);
        REQUIRE(out.size() == 3);
    }

    SECTION("street")
    {
        image = (cv::Mat_<char>(10, 10) <<
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 4, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 5, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 6, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 7, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 8, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 9, 0, 0, 0, 0, 0, 0);

        std::vector<cv::KeyPoint> out;
        fast->setVarThreshold(2);
        fast->detect(image, out);
        REQUIRE(out.size() == 4);
    }

    SECTION("corner")
    {
        image = (cv::Mat_<char>(10, 10) <<
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 4, 4, 0, 0, 0, 0, 0, 0,
            1, 1, 4, 4, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 4, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 0, 0, 0, 0, 0, 0);

        std::vector<cv::KeyPoint> out;
        fast->setVarThreshold(2);
        fast->detect(image, out);
        REQUIRE(out.size() == 1);
    }

    SECTION("many corners")
    {
        image = (cv::Mat_<char>(10, 10) <<
            4, 0, 0, 0, 0, 0, 0, 0, 0, 5,
            0, 4, 0, 0, 0, 0, 0, 0, 5, 0,
            0, 0, 4, 0, 0, 0, 0, 5, 0, 0,
            0, 0, 0, 4, 0, 0, 5, 0, 0, 0,
            0, 0, 0, 0, 4, 5, 0, 0, 0, 0,
            0, 0, 0, 0, 5, 4, 0, 0, 0, 0,
            0, 0, 0, 5, 0, 0, 4, 0, 0, 0,
            0, 0, 5, 0, 0, 0, 0, 4, 0, 0,
            0, 5, 0, 0, 0, 0, 0, 0, 4, 0,
            5, 0, 0, 0, 0, 0, 0, 0, 0, 4);

        std::vector<cv::KeyPoint> out;
        fast->setVarThreshold(2);
        fast->detect(image, out);
        REQUIRE(out.size() == 5);
    }
}
