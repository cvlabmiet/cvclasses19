#include "cvlib.hpp"
#include <catch2/catch.hpp>

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
}

TEST_CASE("Const image", "[corner_detector_fast]")
{
    auto fast = corner_detector_fast::create();
    SECTION("small image")
    {
        const cv::Mat image(6, 6, CV_8UC1, cv::Scalar{15});
        std::vector<cv::KeyPoint> out;
        fast->detect(image, out);
        REQUIRE(out.empty());
    }
    SECTION("no corners")
    {
        const cv::Mat image(10, 10, CV_8UC1, cv::Scalar{15});
        std::vector<cv::KeyPoint> out;
        fast->detect(image, out);
        REQUIRE(out.empty());
    }
    SECTION("3 corners")
    {
        cv::Mat image(10, 10, CV_8UC1, cv::Scalar{15});
        for (int i = 3; i < 7; i++)
            for (int j = 1; j < 6; j++)
            {
                image.at<uint8_t>(j, i) = 100;
            }
        std::vector<cv::KeyPoint> out;
        fast->detect(image, out);
        REQUIRE(out.size() == 5);
    }
    SECTION("1 corner")
    {
        cv::Mat image(10, 10, CV_8UC1, cv::Scalar{15});
        for (int i = 1; i < 6; i++)
            for (int j = 3; j < 7; j++)
            {
                image.at<uint8_t>(j, i) = 100;
            }
        std::vector<cv::KeyPoint> out;
        fast->detect(image, out);
        REQUIRE(out.size() == 1);
    }
    SECTION("1 corner")
    {
        cv::Mat image(10, 10, CV_8UC1, cv::Scalar{15});

        image.at<uint8_t>(3, 3) = 0;
        std::vector<cv::KeyPoint> out;
        fast->detect(image, out);
        REQUIRE(out.size() == 1);
    }
}