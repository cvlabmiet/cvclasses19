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
   corner_detector_fast fast;
   cv::Mat image(10, 10, CV_8UC1);
   SECTION("empty image")
   {
      std::vector<cv::KeyPoint> out;
      fast.detect(image, out);
      REQUIRE(out.empty());
   }

   // \todo add 5 or more tests
}