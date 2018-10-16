/* FAST corner detector algorithm implementation.
* @file
* @date 2018-10-16
* @author Anonymous
*/

#include "cvlib.hpp"

namespace cvlib
{
// static
cv::Ptr<corner_detector_fast> corner_detector_fast::create()
{
   return cv::makePtr<corner_detector_fast>();
}

void corner_detector_fast::detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints,
   cv::InputArray /*mask = cv::noArray()*/)
{
   keypoints.clear();
   // \todo implement FAST with minimal LOCs(lines of code), but keep code readable.
}
} // namespace cvlib
