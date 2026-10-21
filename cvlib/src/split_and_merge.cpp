/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"

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
    if (width <= 1 || height <= 1)
    {
        image.setTo(mean);
        return;
    }

    split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev);
    split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev);
    split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev);
    split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev);
}

void merge_image(cv::Mat image, double stddev)
{
    const auto width = image.cols;
    const auto height = image.rows;

    if (width <= 1 || height <= 1)
        return;

    cv::Mat mean;
    cv::Mat dev;
    cv::meanStdDev(image, mean, dev);

    if (dev.at<double>(0) <= stddev)
    {
        image.setTo(mean);
        return;
    }

    merge_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev);
    merge_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev);
    merge_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev);
    merge_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev);
}
} // namespace

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev)
{
    // split part
    cv::Mat res = image;
    split_image(res, stddev);

    // merge part
    merge_image(res, stddev);
    return res;
}
} // namespace cvlib