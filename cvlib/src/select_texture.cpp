/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Anonymous
 */

#include "cvlib.hpp"

namespace
{

cv::Mat gGaborKernels[3];
int gKernelSize = 0;

void getKernel(int kernel_size)
{
    const double th = CV_PI / 4;
    const double lm = 10.0;
    const double gm = 0.5;
    const double sig[3] = {5, 10, 15};

    for (int i = 0; i < 3; i++)
    {
        gGaborKernels[i] = cv::getGaborKernel(cv::Size(kernel_size, kernel_size), sig[i], th, lm, gm);
    }
}

struct descriptor : public std::vector<double>
{
    using std::vector<double>::vector;

    //descriptor() // ctor
    //{
        //*this = { 0, 0, 0, 0, 0, 0 };
    //}

    descriptor operator-(const descriptor& right) const
    {
        descriptor temp = *this;
        for (int i = 0; i < temp.size(); ++i)
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
        return std::sqrt(res);
    }
};

void calculateDescriptor(const cv::Mat& image,/* int kernel_size,*/ descriptor& descr)
{
    descr.clear();
    const double th = CV_PI / 4;
    const double lm = 10.0;
    const double gm = 0.5;
    cv::Mat response;
    cv::Mat mean;
    cv::Mat dev;

    // \todo implement complete texture segmentation based on Gabor filters
    // (find good combinations for all Gabor's parameters)
    //for (auto sig = 5; sig <= 15; sig += 5)
    for (int i = 0; i < 3; i++)
    {
        //cv::Mat kernel = cv::getGaborKernel(cv::Size(kernel_size, kernel_size), sig, th, lm, gm);
        //cv::filter2D(image, response, CV_32F, kernel);
        cv::filter2D(image, response, CV_32F, gGaborKernels[i]);
        cv::meanStdDev(response, mean, dev);
        //descr[2 * i] = mean.at<double>(0);
        //descr[2 * i + 1] = mean.at<double>(0);
        descr.emplace_back(mean.at<double>(0));
        descr.emplace_back(dev.at<double>(0));
    }
}
} // namespace

namespace cvlib
{
cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps, bool roiChanged)
{
    cv::Mat imROI = image(roi);

    if (roiChanged)
    {
        gKernelSize = int(std::min(roi.height, roi.width) / 4) * 2 + 1;
        getKernel(gKernelSize);
    }

    descriptor reference;
    //calculateDescriptor(image(roi), kernel_size, reference);
    calculateDescriptor(image(roi), reference);

    cv::Mat res = cv::Mat::zeros(image.size(), CV_8UC1);

    descriptor test(reference.size());
    cv::Rect baseROI = roi - roi.tl();

    // \todo move ROI smoothly pixel-by-pixel
    for (int i = 0; i < image.size().width - roi.width; ++i)
    {
        for (int j = 0; j < image.size().height - roi.height; ++j)
        {
            std::cout << "col: " << i << "\trow: " << j << std::endl;
            auto curROI = baseROI + cv::Point(/*roi.width **/ i, /*roi.height **/ j);
            //calculateDescriptor(image(curROI), kernel_size, test);
            calculateDescriptor(image(curROI), test);

            res(curROI) = 255 * ((test - reference).norm_l2() <= eps);
        }
    }

    return res;
}
} // namespace cvlib
