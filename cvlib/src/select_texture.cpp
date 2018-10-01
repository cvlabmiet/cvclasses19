/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <vector>

namespace
{
struct descriptor : public std::vector<double>
{
    using std::vector<double>::vector;
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
            res += std::abs(v) * std::abs(v);
        }
        return std::sqrt(res);
    }
};

class GaborTextureDescriptor
{
    public:
    GaborTextureDescriptor(const cv::Mat& image, const cv::Rect& roi)
    {
        int kernelSize = std::min(roi.height, roi.width) / 2 + 0.5;

        if (kernelSize % 2 == 0)
        {
            if (kernelSize != 0)
                kernelSize--;
            else
                kernelSize++;
        }

        cv::Mat filteredImage;
        cv::Mat quadFilteredImage;
        cv::Mat integralImage;
        cv::Mat quadIntegralImage;

        const double sig[3] = {5, 10, 15};
        const double th[2] = {CV_PI / 4, -CV_PI / 4};
        const double lm = 10;
        const double gm = 0.5;

        for (auto i = 0; i < 3; i++)
        {
            for (auto j = 0; j < 2; j++)
            {
                cv::Mat kernel = cv::getGaborKernel(cv::Size(kernelSize, kernelSize), sig[i], th[j], lm, gm);
                cv::filter2D(image, filteredImage, CV_64F, kernel);

                quadFilteredImage = filteredImage.mul(filteredImage);
                cv::integral(filteredImage, integralImage);
                cv::integral(quadFilteredImage, quadIntegralImage);

                m_integralImages.push_back(integralImage);
                m_quadIntegralImages.push_back(quadIntegralImage);
            }
        }

        calcDescriptor(roi, m_refDescriptor);
    }

    double CalcSimilarity(const cv::Rect& roi)
    {
        descriptor curDescriptor;
        calcDescriptor(roi, curDescriptor);
        return (curDescriptor - m_refDescriptor).norm_l2();
    }

    private:
    void calcDescriptor(const cv::Rect& roi, descriptor& descr)
    {
        double mean, quadMean, stddev;
        size_t pixelNum = roi.width * roi.height;

        for (size_t i = 0; i < m_integralImages.size(); i++)
        {
            mean = calcSummForRect(m_integralImages[i], roi) / pixelNum;
            descr.emplace_back(mean);

            quadMean = calcSummForRect(m_quadIntegralImages[i], roi) / pixelNum;
            ;
            stddev = std::sqrt(quadMean - mean * mean);
            descr.emplace_back(stddev);
        }
    }

    double calcSummForRect(const cv::Mat& integral, const cv::Rect& rect)
    {
        return integral.at<double>(rect.y + rect.height, rect.x + rect.width) - integral.at<double>(rect.y, rect.x + rect.width) -
               integral.at<double>(rect.y + rect.height, rect.x) + integral.at<double>(rect.y, rect.x);
    }

    std::vector<cv::Mat> m_integralImages;
    std::vector<cv::Mat> m_quadIntegralImages;
    descriptor m_refDescriptor;
};
} // namespace

namespace cvlib
{
cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps)
{
    if (image.empty())
        return image;

    if (std::min(roi.height, roi.width) < 1)
        return cv::Mat::zeros(image.size(), CV_8UC1);

    GaborTextureDescriptor gaborTextureDescriptor(image, roi);

    cv::Mat res = cv::Mat::zeros(image.size(), CV_8UC1);
    cv::Rect baseRoi = roi - roi.tl();
    cv::Rect curRoi;
    double similarity;

    for (size_t i = 0; i <= image.rows - roi.height; i++)
    {
        for (size_t j = 0; j <= image.cols - roi.width; j++)
        {
            curRoi = baseRoi + cv::Point(j, i);
            similarity = gaborTextureDescriptor.CalcSimilarity(curRoi);
            res(curRoi) += 255 * (similarity <= eps);
        }
    }

    return res;
}
} // namespace cvlib
