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
    SECTION("empty image")
    {
        cv::Mat image(10, 10, CV_8UC1, cv::Scalar(0));
        std::vector<cv::KeyPoint> out;
        fast->detect(image, out);
        REQUIRE(out.empty());
    }
    SECTION("small image")
    {
        const auto image = cv::Mat(3, 3, CV_8UC1, cv::Scalar(127));
        std::vector<cv::KeyPoint> out;
        fast->detect(image, out);
        REQUIRE(out.empty());
    }
    SECTION("1 corner")
    {
        // clang-format off
        cv::Mat image = (cv::Mat_<uint8_t>(7, 7) <<
                127, 127, 127, 127, 127, 127, 127,
                255, 127, 127, 127, 127, 127, 127,
                255, 255, 255, 127, 127, 127, 127,
                255, 255, 255, 255, 127, 127, 127,
                255, 255, 255, 127, 127, 127, 127,
                255, 127, 127, 127, 127, 127, 127,
                127, 127, 127, 127, 127, 127, 127
                );
        // clang-format on
        std::vector<cv::KeyPoint> out;
        fast->detect(image, out);
        REQUIRE(1 == out.size());
    }
}
