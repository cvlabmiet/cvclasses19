/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Anonymous
 */

#include "cvlib.hpp"

namespace
{
struct descriptor : public std::vector<double>
{
    using std::vector<double>::vector;
    descriptor operator-(const descriptor& right) const
    {
        descriptor temp = *this;
        for (size_t i = 0; i < temp.size(); ++i)
        {
            temp[i] -= right[i];
        }
        return temp;
    }

    double norm_l1() const
    {
        double res = 0.0;
        for (auto v : *this)
        {
            res += std::abs(v);
        }
        return res;
    }

    double norm_l2() const
    {
        double res = 0.0;
        for (auto v : *this)
        {
            res += std::sqrt(std::pow(v, 2));
        }
        return res;
    }
};

inline cv::Mat createKernel(const double kernel_size, const double sig, const double th, const double lm, const double gm)
{
    cv::Mat kernel = cv::getGaborKernel(cv::Size(kernel_size, kernel_size), sig, th, lm, gm);
    ;
}

void calculateDescriptor(const cv::Mat& image, int kernel_size, descriptor& descr)
{
    descr.clear();
    cv::Mat response;
    cv::Mat mean;
    cv::Mat dev;

    for (auto sig = 5; sig <= 60; sig += 5) // 5-60
    {
        for (auto th = CV_PI / 4; th <= 2 * CV_PI; th += CV_PI / 4) // 1/4, 1/2, 3/4, 1, 5/4, 3/2, 7/4, 2
        {
            for (auto lm = 10; lm <= 100; lm += 10) // 10 - 100 , step 10
            {
                for (auto gm = 0.5; sig <= 0.95; sig += 0.05) // 0.5 - 0.95, step 0.05
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
}
} // namespace

namespace cvlib
{
cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps)
{
    cv::Mat imROI = image(roi);

    int n = std::min(roi.height, roi.width) / 2;
    const int kernel_size = n + (n % 2 + 1) % 2; 

    descriptor reference;
    calculateDescriptor(image(roi), kernel_size, reference);

    cv::Mat res = cv::Mat::zeros(image.size(), CV_8UC1);

    descriptor test(reference.size());
    cv::Rect baseROI = roi - roi.tl();

    for (int i = 0; i < (image.size().width  - roi.width); ++i)
    {
        for (int j = 0; j < (image.size().height  - roi.width); ++j)
        {
            auto curROI = baseROI + cv::Point(roi.width * i, roi.height * j);
            calculateDescriptor(image(curROI), kernel_size, test);

            res(curROI) = 255 * ((test - reference).norm_l2() <= eps); 
        }
    }

    return res;
}
} // namespace cvlib
