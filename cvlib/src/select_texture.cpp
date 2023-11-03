/* Texture Selection algorithm implementation
 * @file
 * @date 2023-10-25
 * @author Yaroslav Murenkov
 */

#include "cvlib.hpp"

#include <cstdint>
#include <cstdlib>

namespace
{
struct descriptor : public std::vector<double>
{
    using std::vector<double>::vector;
    descriptor operator-(const descriptor& right) const
    {
        descriptor temp = *this;
        for (size_t i = 0; i < temp.size(); ++i)
            temp[i] -= right[i];
        return temp;
    }

    double norm_l1() const
    {
        double res = 0.0;
        for (auto v : *this)
            res += std::abs(v);
        return res;
    }

    double norm_l2() const
    {
        double res = 0.0;
        for (auto v : *this)
            res += v * v;
        return sqrt(res);
    }
};

void calculateDescriptor(const cv::Mat& image, int32_t kernel_size, descriptor& descr)
{
    descr.clear();
    const double gm = 0.5;
    cv::Mat response;
    cv::Mat mean;
    cv::Mat dev;

    for (auto lm = 3; lm < 15; lm += 5)
    {
        for (double th = CV_PI / 4; th < CV_PI; th += CV_PI / 4)
        {
            for (auto sig = 5; sig <= 15; sig += 5)
            {
                cv::Mat kernel = cv::getGaborKernel(cv::Size(kernel_size, kernel_size), sig, th, lm, gm);
                cv::filter2D(image, response, CV_32F, kernel);
                cv::meanStdDev(response, mean, dev);
                descr.emplace_back(mean.at<double>(0));
                descr.emplace_back(dev.at<double>(0));
            }
        }
    }
}
} // namespace

namespace cvlib
{
cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps)
{
    cv::Mat imROI = image(roi);

    const int32_t kernel_size = (std::min(roi.height, roi.width) / 2) | 0b1;

    descriptor reference;
    calculateDescriptor(image(roi), kernel_size, reference);

    cv::Mat res = cv::Mat::zeros(image.size(), CV_8UC1);

    descriptor test(reference.size());
    cv::Rect baseROI = roi - roi.tl();

    // \todo move ROI smoothly pixel-by-pixel
    for (size_t i = 0; i < image.size().width / roi.width; ++i)
    {
        for (size_t j = 0; j < image.size().height / roi.height; ++j)
        {
            auto curROI = baseROI + cv::Point(roi.width * i, roi.height * j);
            calculateDescriptor(image(curROI), kernel_size, test);

            res(curROI) = 255 * ((test - reference).norm_l1() <= eps);
        }
    }

    return res;
}
} // namespace cvlib
