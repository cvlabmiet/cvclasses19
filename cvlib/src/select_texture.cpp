/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Anonymous
 */

#include "cvlib.h"
#include <chrono>

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
            res += v*v;
        }
        return sqrt(res);
    }
};

void calculateDescriptor(const cv::Mat& image, int kernel_size, descriptor& descr)
{
    descr.clear();
    //const double th = CV_PI / 4;
    //const double lm = 10.0;
    const double gm = 0.5;
    cv::Mat response;
    cv::Mat mean;
    cv::Mat dev;

    // \todo implement complete texture segmentation based on Gabor filters
    // (find good combinations for all Gabor's parameters) (completed)
    for (auto lm = 3; lm < 15; lm += 5)
    {
        for (double th = CV_PI/4; th < CV_PI; th += CV_PI/4)
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

    int kernel_size = std::min(roi.height, roi.width) / 2; // \todo round to nearest odd (completed)
    if (kernel_size % 2)
    {
        kernel_size -= 1;
    }

    descriptor reference;
    calculateDescriptor(image(roi), kernel_size, reference);

    cv::Mat res = cv::Mat::zeros(image.size(), CV_8UC1);

    descriptor test(reference.size());
    cv::Rect baseROI = roi - roi.tl();

auto time_start = std::chrono::high_resolution_clock::now();

    // \todo move ROI smoothly pixel-by-pixel (completed)
    for (int i = 0; i < image.size().width - roi.width; i+= 20)
    {

        for (int j = 0; j < image.size().height - roi.height; j+= 20)
        {
            auto curROI = baseROI + cv::Point(i, j);
            calculateDescriptor(image(curROI), kernel_size, test);

            // \todo implement and use norm L2 (completed)
            res(curROI) = 255 * ((test - reference).norm_l2() <= eps);
        }
    }

auto time_end = std::chrono::high_resolution_clock::now();
std::cout << "Elapsed time in ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count() << std::endl;


    return res;
}
} // namespace cvlib
