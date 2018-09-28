/* @brief   Texture segmentation algorithm implementation.
 * @file    select_texture.cpp
 * @date    2018-09-28
 * @author  AWESOME KOSTYAN
 */

#include "cvlib.hpp"

#define FILTERS_NUMBER 3
#define FEATURES_NUMBER 2
#define DESCRIPTOR_SIZE FILTERS_NUMBER* FEATURES_NUMBER

namespace
{
int gKernelSize = 0;
cv::Mat gGaborKernels[FILTERS_NUMBER];
cv::Mat gFilteredImages[FILTERS_NUMBER];

void getKernel(int kernel_size)
{
    const double th[FILTERS_NUMBER] = {0, CV_PI / 4, CV_PI / 2};
    const double lm = 10.0; // 4.0;//10.0;
    const double gm = 0.75; // 0.15;//0.5;
    const double sig = 1.0;

    for (int i = 0; i < FILTERS_NUMBER; i++)
        gGaborKernels[i] = cv::getGaborKernel(cv::Size(kernel_size, kernel_size), sig, th[i], lm, gm);
}

void filterImage(const cv::Mat& image)
{
    for (int i = 0; i < FILTERS_NUMBER; i++)
        cv::filter2D(image, gFilteredImages[i], CV_32F, gGaborKernels[i]);
}

struct Descriptor
{
    Descriptor()
    {
        this->clear();
    }

    Descriptor operator-(const Descriptor& right) const
    {
        Descriptor temp = *this;
        for (int i = 0; i < DESCRIPTOR_SIZE; ++i)
        {
            temp.m_descriptor[i] -= right.m_descriptor[i];
        }
        return temp;
    }

    void clear()
    {
        for (int i = 0; i < DESCRIPTOR_SIZE; ++i)
            m_descriptor[i] = 0;
    }

    double norm_l2() const
    {
        double res = 0.0;
        for (int i = 0; i < DESCRIPTOR_SIZE; ++i)
        {
            res += m_descriptor[i] * m_descriptor[i];
        }
        return res;
    }

    double m_descriptor[DESCRIPTOR_SIZE];
};

void calculateDescriptor(const cv::Rect& roi, Descriptor& descr)
{
    descr.clear();
    cv::Mat mean;
    cv::Mat dev;

    for (int i = 0; i < FILTERS_NUMBER; i++)
    {
        cv::meanStdDev(gFilteredImages[i](roi), mean, dev);
        descr.m_descriptor[2 * i] = mean.at<double>(0);
        descr.m_descriptor[2 * i + 1] = dev.at<double>(0);
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

    Descriptor reference;
    Descriptor test;
    cv::Rect baseROI = roi - roi.tl();

    filterImage(image);
    calculateDescriptor(roi, reference);
    cv::Mat res = cv::Mat::zeros(image.size(), CV_8UC1);
    cv::Mat mask = cv::Mat::zeros(image.size(), CV_8UC1);

    for (int i = 0; i < image.size().width - roi.width; ++i)
    {
        for (int j = 0; j < image.size().height - roi.height; ++j)
        {
            if (mask.at<unsigned char>(j, i) == 1)
                continue;

            auto curROI = baseROI + cv::Point(i, j);
            calculateDescriptor(curROI, test);

            int f = ((test - reference).norm_l2() <= eps * eps);
            if (f != 0)
            {
                res(curROI) = 255 * f;
                mask(curROI) = 1;
                j += gKernelSize - 1;
            }
        }
    }

    return res;
}
} // namespace cvlib
