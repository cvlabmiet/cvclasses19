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

void corner_detector_fast::detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray /*mask = cv::noArray()*/)
{
    keypoints.clear();

	if (image.empty())
		return;

	else if (image.channels() == 1)
		image.copyTo(image_);

	else if (image.channels() == 3)
		cv::cvtColor(image, image_, cv::COLOR_BGR2GRAY);

	for (auto y = 3; y < image_.rows - 3; ++y)
		for (auto x = 3; x < image_.cols - 3; ++x)
			if (is_keypoint(cv::Point(x, y), 4, 3) &&
				is_keypoint(cv::Point(x, y), 1, 12))
				keypoints.push_back(cv::KeyPoint(cv::Point(x, y), 1.f));
}

void corner_detector_fast::set_threshold(int thresh)
{
	threshold_ = thresh;
}

brightness_check_result corner_detector_fast::check_brightness(unsigned int circle_point_num, cv::Point center)
{
	uchar center_brightness = image_.at<uchar>(center);
	uchar circle_point_brightness = image_.at<uchar>(center + circle_template_[circle_point_num]);
	if (circle_point_brightness <= center_brightness - static_cast<uchar>(threshold_))
		return brightness_check_result::darker;

	if(center_brightness + static_cast<uchar>(threshold_) <= circle_point_brightness)
		return brightness_check_result::brighter;

	if ((circle_point_brightness > center_brightness - static_cast<uchar>(threshold_)) &&
		(circle_point_brightness < center_brightness + static_cast<uchar>(threshold_)))
		return brightness_check_result::similar;
}

bool corner_detector_fast::is_keypoint(cv::Point center, unsigned int step, unsigned int num)
{
	unsigned int brighter_count = 0;
	unsigned int darker_count = 0;

	for (int i = 0; i < 16; i += step)
	{
		brightness_check_result res = check_brightness(i, center);

		switch (res)
		{
		case brightness_check_result::brighter:
			++brighter_count;
			break;
		case brightness_check_result::darker:
			++darker_count;
			break;
		dafault:
			break;
		}
	}

	return ((brighter_count >= num) || (darker_count >= num));
}
} // namespace cvlib
