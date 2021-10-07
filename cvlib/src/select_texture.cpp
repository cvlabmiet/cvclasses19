/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Anonymous
 */

#include "cvlib.hpp"

namespace
{
    std::vector<cv::Mat> filter_kernels; 
    std::vector<cv::Mat> feture_maps;

struct Descriptor : public std::vector<double>
{
    using std::vector<double>::vector;
    Descriptor operator-(const Descriptor& right) const
    {
        Descriptor temp = *this;
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
            res += v*v;
        res = std::sqrt(res);
        return res;
    }
};

void calculateReferenceDescriptor(const cv::Mat& image, Descriptor& descr)
{
    descr.clear();
    cv::Mat response;
    cv::Mat mean;
    cv::Mat dev;

    for (const auto& kernel : filter_kernels)
    {
        cv::filter2D(image, response, CV_32F, kernel);
        cv::meanStdDev(response, mean, dev);
        descr.emplace_back(mean.at<double>(0));
        descr.emplace_back(dev.at<double>(0));
    }
}

void calculateDescriptorFromMaps(const cv::Rect& roi, Descriptor& descr)
{   
    descr.clear();
    cv::Mat mean;
    cv::Mat dev;
    for(const auto& map : feture_maps)
    {
        cv::meanStdDev(map(roi), mean, dev);
        descr.emplace_back(mean.at<double>(0));
        descr.emplace_back(dev.at<double>(0));
    }
}

void calculateFeatureMaps(const cv::Mat& image)
{
    if(filter_kernels.empty()) return;

    feture_maps.clear();
    for (const auto& kernel : filter_kernels)
    {
        feture_maps.emplace_back();
        cv::filter2D(image, feture_maps.back(), CV_32F, kernel);
    }
}

void calculateFilters(int kernel_size)
{
    // \todo implement complete texture segmentation based on Gabor filters
    // (find good combinations for all Gabor's parameters)
    filter_kernels.clear();
    const double lm = 10.0;
    const double gm = 0.5;
    const double step_sig = kernel_size/12 == 0 ? 1 : kernel_size/12; 
    //for(auto lm = 5.0; lm <= 15.0; lm += 3)
        for (auto th = CV_PI /4; th <= CV_PI; th += CV_PI /4)
            for (auto sig = step_sig; sig <= 3*step_sig; sig += step_sig)
                filter_kernels.push_back(cv::getGaborKernel(cv::Size(kernel_size, kernel_size), sig, th, lm, gm));
}

} // namespace

namespace cvlib
{
cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps)
{
    double num = static_cast<double>(std::min(roi.height, roi.width)) / 2.0;
    const int kernel_size = std::floor(num) / 2 != 0 ? std::floor(num) : std::ceil(num);
    static int prev_kernel_size;
    if(prev_kernel_size != kernel_size) 
    {
        calculateFilters(kernel_size);
        prev_kernel_size = kernel_size;
        std::cout << filter_kernels.size() << std::endl;
    }
    calculateFeatureMaps(image);
    
    Descriptor reference;
    calculateReferenceDescriptor(image(roi), reference);

    cv::Mat res = cv::Mat::zeros(image.size(), CV_8UC1);

    Descriptor test(reference.size());
    cv::Rect baseROI = roi - roi.tl();

    int step_h = 10;
    int step_w = 10;

    // \todo move ROI smoothly pixel-by-pixel --> imposible
    for (int i = 0; i < image.size().width - roi.width; i += step_w)
    {
        for (int j = 0; j < image.size().height - roi.height; j += step_h)
        {
            auto curROI = baseROI + cv::Point(i, j);
            calculateDescriptorFromMaps(curROI, test);

            res(curROI) = 255 * ((test - reference).norm_l2() <= eps);
        }
    }

    return res;
}
} // namespace cvlib
