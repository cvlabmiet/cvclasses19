/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2023-11-14
 * @author Yaroslav Murenkov
 */

#include "cvlib.hpp"

namespace
{
void split_image(cv::Mat image, double stddev)
{
    cv::Mat mean;
    cv::Mat dev;

    const auto width = image.cols;
    const auto height = image.rows;

    if (width == 1 || height == 1)
        return;

    cv::meanStdDev(image, mean, dev);
    if (dev.at<double>(0) <= stddev)
    {
        image.setTo(mean);
        return;
    }

    split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev);
    split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev);
    split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev);
    split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev);
}

void merge_image(cv::Mat image, double stddev)
{
    cv::Mat image_ROI;
    cv::Mat mean;
    cv::Mat dev;

    const auto width = image.cols;
    const auto height = image.rows;

    if (width == 1 || height == 1)
        return;

    image_ROI = image(cv::Range(0, height / 2), cv::Range(0, width));
    cv::meanStdDev(image_ROI, mean, dev);
    if (dev.at<double>(0) <= stddev)
    {
        image_ROI.setTo(mean);
        return;
    }

    image_ROI = image(cv::Range(height / 2, height), cv::Range(0, width));
    cv::meanStdDev(image_ROI, mean, dev);
    if (dev.at<double>(0) <= stddev)
    {
        image_ROI.setTo(mean);
        return;
    }

    image_ROI = image(cv::Range(0, height), cv::Range(0, width / 2));
    cv::meanStdDev(image_ROI, mean, dev);
    if (dev.at<double>(0) <= stddev)
    {
        image_ROI.setTo(mean);
        return;
    }

    image_ROI = image(cv::Range(0, height), cv::Range(width / 2, width));
    cv::meanStdDev(image_ROI, mean, dev);
    if (dev.at<double>(0) <= stddev)
    {
        image_ROI.setTo(mean);
        return;
    }

    merge_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev);
    merge_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev);
    merge_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev);
    merge_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev);
}
} // namespace

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev)
{
    cv::Mat res = image;
    split_image(res, stddev);

    merge_image(res, stddev);
    return res;
}
} // namespace cvlib
