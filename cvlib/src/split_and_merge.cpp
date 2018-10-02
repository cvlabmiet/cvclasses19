/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <map>

#define MIN_SPLIT_SIZE 5
#define DELTA_MEAN 10



std::map<double, std::pair<cv::Range, cv::Range>> region_history;

cv::Mat integralImg, sqIntegralImg;

namespace
{

// mean and std on integral images 
void split_region(int x1, int y1, int x2, int y2, const double& stddev)
{
	int S1 = integralImg.at<int>(y2, x2) + integralImg.at<int>(y1, x1) - integralImg.at<int>(y2, x1) - integralImg.at<int>(y1, x2);
	double S2 = sqIntegralImg.at<double>(y2, x2) + sqIntegralImg.at<double>(y1, x1) - sqIntegralImg.at<double>(y2, x1) - sqIntegralImg.at<double>(y1, x2);
	double mean = double(S1) / (x2 - x1) / (y2 - y1);
	double std = sqrt(1.0 / (x2 - x1) / (y2 - y1) * (S2 - 2 * mean * S1) + mean * mean);

	if (std <= stddev || (x2 - x1) < MIN_SPLIT_SIZE || (y2 - y1) < MIN_SPLIT_SIZE)
	{
		region_history.emplace(mean, std::pair<cv::Range, cv::Range>(cv::Range(y1, y2), cv::Range(x1, x2)));
		return;
	}

	split_region(x1, y1, (x1 + x2) / 2, (y1 + y2) / 2, stddev);
	split_region((x1 + x2) / 2, y1, x2, (y1 + y2) / 2, stddev);
	split_region(x1, (y1 + y2) / 2, (x1 + x2) / 2, y2, stddev);
	split_region((x1 + x2) / 2, (y1 + y2) / 2, x2, y2, stddev);
}

void split_image(cv::Mat& image, const double& stddev)
{
	cv::Mat int_img, squared_int_img;
	cv::integral(image, integralImg, sqIntegralImg);

	split_region(0, 0, image.cols, image.rows, stddev);
}

void merge_image(cv::Mat& image)
{
	auto elem = region_history.begin();
	double next_val = elem->first + DELTA_MEAN;
	while (elem != region_history.end())
	{
		if (elem->first > next_val)
			next_val += DELTA_MEAN;
		image(elem->second.first, elem->second.second).setTo(int(next_val - DELTA_MEAN / 2));
		elem++;
	}
	
}
} // namespace

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev)
{
    // split part
    cv::Mat res = image; // copy
    split_image(res, stddev);

    // merge part
	merge_image(res);

	// clear global vars
	integralImg.release();
	sqIntegralImg.release();
	region_history.clear();

    return res;
}
} // namespace cvlib
