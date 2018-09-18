/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <iostream>

namespace
{
	
void split_image(cv::Mat image, double stddev)
{
    cv::Mat mean;
    cv::Mat dev;
    cv::meanStdDev(image, mean, dev);

    if (dev.at<double>(0) <= stddev)
    {
        image.setTo(mean);
        return;
    }

    const auto width = image.cols;
    const auto height = image.rows;

    split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev);
    split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev);
    split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev);
    split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev);
}

void split_and_merge_image(cv::Mat image, double stddev, double diffmean)
{
	/*
	Regions and delimeters:

	        0
		    |
		R0  |  R1
		    |
    3---------------1
		    |
		R3  |  R2
		    |
		    2
	*/
	cv::Mat mean;
	cv::Mat dev;
	cv::meanStdDev(image, mean, dev);

	if (dev.at<double>(0) <= stddev)
	{
		image.setTo(mean);
		return;
	}

	const auto width = image.cols;
	const auto height = image.rows;

	cv::Mat R0 = image(cv::Range(0, height / 2), cv::Range(0, width / 2));
	cv::Mat R1 = image(cv::Range(0, height / 2), cv::Range(width / 2, width));
	cv::Mat R2 = image(cv::Range(height / 2, height), cv::Range(width / 2, width));
	cv::Mat R3 = image(cv::Range(height / 2, height), cv::Range(0, width / 2));

	cv::Mat R0mean;
	cv::Mat R1mean;
	cv::Mat R2mean;
	cv::Mat R3mean;
	cv::Mat std;

	cv::meanStdDev(R0, R0mean, std);
	double R0std = std.at<double>(0);
	cv::meanStdDev(R1, R1mean, std);
	double R1std = std.at<double>(0);
	cv::meanStdDev(R2, R2mean, std);
	double R2std = std.at<double>(0);
	cv::meanStdDev(R3, R3mean, std);
	double R3std = std.at<double>(0);

	bool u0 = false; // unions: u0 - unite R0 and R1
	bool u1 = false;
	bool u2 = false;
	bool u3 = false;

	if (R0std <= stddev && R1std <= stddev
		&& std::abs(R0mean.at<double>(0) - R1mean.at<double>(0)) <= diffmean)
	{
		u0 = true;
		R0.setTo((R0mean + R1mean) / 2);
		R1.setTo((R0mean + R1mean) / 2);
	}

	if (R1std <= stddev && R2std <= stddev && !u0
		&& std::abs(R1mean.at<double>(0) - R2mean.at<double>(0)) <= diffmean)
	{
		u1 = true;
		R1.setTo((R1mean + R2mean) / 2);
		R2.setTo((R1mean + R2mean) / 2);
	}

	if (R2std <= stddev && R3std <= stddev && !u1
		&& std::abs(R2mean.at<double>(0) - R3mean.at<double>(0)) <= diffmean)
	{
		u2 = true;
		R2.setTo((R2mean + R3mean) / 2);
		R3.setTo((R2mean + R3mean) / 2);
	}

	if (R0std <= stddev && R3std <= stddev && !u0 && !u2
		&& std::abs(R0mean.at<double>(0) - R3mean.at<double>(0)) <= diffmean)
	{
		u3 = true;
		R0.setTo((R0mean + R3mean) / 2);
		R3.setTo((R0mean + R3mean) / 2);
	}

	if (!u0 && !u3) split_and_merge_image(R0, stddev, diffmean);
	if (!u0 && !u1) split_and_merge_image(R1, stddev, diffmean);
	if (!u1 && !u2) split_and_merge_image(R2, stddev, diffmean);
	if (!u2 && !u3) split_and_merge_image(R3, stddev, diffmean);
}

}

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev, double diffmean)
{
    // split part
    cv::Mat res = image;
    //split_image(res, stddev);
    split_and_merge_image(res, stddev, diffmean);

    // merge part
    // \todo implement merge algorithm
    return res;
}

cv::Mat split_only(const cv::Mat& image, double stddev)
{
	// split part
	cv::Mat res = image;
	split_image(res, stddev);

	// merge part
	// \todo implement merge algorithm
	return res;
}
} // namespace cvlib
