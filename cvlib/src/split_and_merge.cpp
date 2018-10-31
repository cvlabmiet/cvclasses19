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
void split_image(cv::Mat image, int x0, int y0, int x1, int y1, double stddev)
{
    int S1 = integralImg.at<int>(y1, x1) + integralImg.at<int>(y0, x0) - integralImg.at<int>(y1, x0) - integralImg.at<int>(y0, x1);
    double S2 = sqIntegralImg.at<double>(y1, x1) + sqIntegralImg.at<double>(y0, x0) - sqIntegralImg.at<double>(y1, x0) - sqIntegralImg.at<double>(y0, x1);
    double mean = double(S1) / (x1 - x0) / (y1 - y0);
    double std = sqrt(1.0 / (x1 - x0) / (y1 - y0) * (S2 - 2 * mean * S1) + mean * mean);

    if (std <= stddev || abs(x1 - x0) < MIN_SPLIT_SIZE || abs(y1 - y0) < MIN_SPLIT_SIZE)
    {
        region_history.emplace(mean, std::pair<cv::Range, cv::Range>(cv::Range(y0, y1), cv::Range(x0, x1)));
        image.setTo(mean);
        return;
    }
    const auto width = image.cols;
    const auto height = image.rows;

    cv::Mat R0 = image(cv::Range(0, height / 2), cv::Range(0, width / 2));
    cv::Mat R1 = image(cv::Range(0, height / 2), cv::Range(width / 2, width));
    cv::Mat R2 = image(cv::Range(height / 2, height), cv::Range(width / 2, width));
    cv::Mat R3 = image(cv::Range(height / 2, height), cv::Range(0, width / 2));

    split_image(R0, x0, y0, (x0 + x1) / 2, (y0 + y1) / 2, stddev);
    split_image(R1, (x1 + x0) / 2, y0, x1, (y0 + y1) / 2, stddev);
    split_image(R2, (x1 + x0) / 2, (y0 + y1) / 2, x1, y1, stddev);
    split_image(R3, x0, (y0 + y1) / 2, (x0 + x1) / 2, y1, stddev);
}
} // namespace

void merge_image(cv::Mat image, double diffdev)
{
    auto elem = region_history.begin();
    double mean_val = elem->first + diffdev;
    while (elem != region_history.end())
    {
        if (elem->first > mean_val)
            mean_val += diffdev;
        int new_val = mean_val - diffdev / 2;
        if (new_val > 255)
            new_val = 255;
        image(elem->second.first, elem->second.second).setTo(new_val);
        elem++;
    }
}

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev, double diffdev)
{
    // split part
    cv::Mat res = image;
    cv::Mat int_img, squared_int_img;
    cv::integral(image, integralImg, sqIntegralImg);

    split_image(res, 0, 0, image.cols, image.rows, stddev);

    // merge part
    merge_image(res, diffdev);

    integralImg.release();
    sqIntegralImg.release();
    region_history.clear();

    return res;
} // namespace cvlibcv::Matsplit_and_merge(constcv::Mat&image,doublestddev)
} // namespace cvlib
