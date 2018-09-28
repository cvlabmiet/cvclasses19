/* Texture Segmentation.
 * @file
 * @date 2018-09-28
 * @author Dmitrij Vukalov
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
            res += v * v;
        }
        return res;
    }
};

const unsigned int NUM_FILTERS = 3;
unsigned int KERNEL_SIZE = 0;

class TextureSegmentation
{
    public:
    cv::Mat selectTexture(const cv::Mat& image, const cv::Rect& roi, double eps)
    {
        m_textureDescr.clear();

        image.copyTo(m_image);
        m_roi = roi;

        int currSize = getKernelSize(roi);

        if (currSize != m_kernelSize)
        {
            gaborFilters();
            m_kernelSize = currSize;
        }

        filterImage();

        calculateDescriptorForRoi(m_image(m_roi));

        descriptor test(m_textureDescr.size());

        cv::Rect baseROI = m_roi - m_roi.tl();

        cv::Mat res = cv::Mat::zeros(m_image.size(), CV_8UC1);

        for (int i = 0; i < m_image.size().width - m_roi.width; ++i)
        {
            for (int j = 0; j < m_image.size().height - m_roi.height; ++j)
            {
                auto curROI = baseROI + cv::Point(i, j);

                calculateDescriptor(curROI, test);

                int a = ((test - m_textureDescr).norm_l2() <= eps * eps);

                if (a > 0)
                {
                    res(curROI) = 255 * a;
                    j += m_kernelSize - 1;
                }
                // res(curROI) = 255 * ((test - m_textureDescr).norm_l2() <= eps);
            }
        }
        return res;
    }

    private:
    int getKernelSize(const cv::Rect& roi)
    {
        return int(std::min(roi.height, roi.width) / 4) * 2 + 1;
    }

    void gaborFilters()
    {
        // const double th = CV_PI / 4;
        const double th[NUM_FILTERS] = {0, CV_PI / 4, CV_PI / 2};
        const double lm = 10.0;
        const double gm = 0.5;
        const double sig = {1};
        // const double sig[NUM_FILTERS] = { 5, 10, 15 };

        int i = 0;
        for (auto i = 0; i < NUM_FILTERS; i++)
        {
            m_kernelFilter[i] = cv::getGaborKernel(cv::Size(m_kernelSize, m_kernelSize), sig, th[i], lm, gm);
        }
    }

    void filterImage()
    {
        for (auto i = 0; i < NUM_FILTERS; i++)
        {
            cv::filter2D(m_image, m_filteredImages[i], CV_32F, m_kernelFilter[i]);
        }
    }

    void calculateDescriptorForRoi(const cv::Mat& image)
    {
        cv::Mat response;

        cv::Mat mean;
        cv::Mat dev;

        for (int i = 0; i < NUM_FILTERS; i++)
        {
            cv::meanStdDev(m_filteredImages[i](m_roi), mean, dev);
            m_textureDescr.emplace_back(mean.at<double>(0));
            m_textureDescr.emplace_back(dev.at<double>(0));
        }
    }

    void calculateDescriptor(const cv::Rect& roi, descriptor& descr)
    {
        descr.clear();
        cv::Mat mean;
        cv::Mat dev;

        for (auto i = 0; i < NUM_FILTERS; i++)
        {
            cv::meanStdDev(m_filteredImages[i](roi), mean, dev);
            descr.emplace_back(mean.at<double>(0));
            descr.emplace_back(dev.at<double>(0));
        }
    }

    descriptor m_textureDescr;
    descriptor m_descr;
    cv::Mat m_image;
    cv::Mat m_kernelFilter[NUM_FILTERS];
    cv::Mat m_filteredImages[NUM_FILTERS];
    cv::Rect m_roi;
    int m_kernelSize;
};
} // namespace

namespace cvlib
{
TextureSegmentation test;
cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps)
{
    cv::Mat a = test.selectTexture(image, roi, eps);
    return a;
}
} // namespace cvlib
