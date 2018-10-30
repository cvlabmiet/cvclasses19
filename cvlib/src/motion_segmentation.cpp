/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Darina Kalina
 */

#include "cvlib.hpp"

#include <iostream>

namespace cvlib
{

void motion_segmentation::setVarThreshold(double threshold)
{
	var_threshold_ = threshold;
}
void motion_segmentation::apply(cv::InputArray _image, cv::OutputArray _fgmask, double leaning_rate)
{
	if (_image.empty())
	{
		_image.copyTo(_fgmask);
		return;
	}

	cv::Mat converted;
	cv::Mat image;
	_image.getMat().convertTo(converted, CV_32F);
	cv::medianBlur(converted, image, 5);

	if (current_frame_ == 0)
	{
		bg_model_ = cv::Mat::zeros(image.size(), CV_32F);
		cv::Scalar scalar_mean = cv::mean(image);
		cv::Scalar square_scalar_mean = cv::mean(image.mul(image));
		float var = square_scalar_mean.val[0] - scalar_mean.val[0] * scalar_mean.val[0];
		variance_ = cv::Mat(image.size(), CV_32F, cv::Scalar(var));
	}

	++current_frame_;

	cv::Mat diff;
	cv::absdiff(image, bg_model_, diff);
	cv::accumulateWeighted(image, bg_model_, leaning_rate);

	if (current_frame_ == 2)
	{
		is_initialized_ = true;
	}

	cv::Mat square_diff = diff.mul(diff);
	cv::Mat stddev;
	cv::sqrt(variance_, stddev);

	cv::Mat result = ((diff / stddev) > var_threshold_) * 255;
	cv::accumulateWeighted(square_diff, variance_, leaning_rate);
	_fgmask.assign(result);
}
} // namespace cvlib
