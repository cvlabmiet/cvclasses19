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
            // \todo битовыми сдвигами
            res += std::pow(v, 2);
        }
        return res;
    }
};

void convolveDFT(cv::InputArray A, cv::InputArray B, cv::OutputArray C)
{
    C.create(abs(A.rows() - B.rows()) + 1, abs(A.cols() - B.cols()) + 1, A.type());
    cv::Size dftSize;
    // calculate the size of DFT transform
    dftSize.width = cv::getOptimalDFTSize(A.cols() + B.cols() - 1);
    dftSize.height = cv::getOptimalDFTSize(A.rows() + B.rows() - 1);
    // allocate temporary buffers and initialize them with 0's
    cv::Mat tempA(dftSize, A.type(), cv::Scalar::all(0));
    cv::Mat tempB(dftSize, B.type(), cv::Scalar::all(0));
    // copy A and B to the top-left corners of tempA and tempB, respectively
    cv::Mat roiA(tempA, cv::Rect(0, 0, A.cols(), A.rows()));
    A.copyTo(roiA);
    cv::Mat roiB(tempB, cv::Rect(0, 0, B.cols(), B.rows()));
    B.copyTo(roiB);
    // now transform the padded A & B in-place;
    // use "nonzeroRows" hint for faster processing
    cv::dft(tempA, tempA, 0, A.rows());
    cv::dft(tempB, tempB, 0, B.rows());

    // multiply the spectrums;
    // the function handles packed spectrum representations well
    cv::mulSpectrums(tempA, tempB, tempA, 0);
    // transform the product back from the frequency domain.
    // Even though all the result rows will be non-zero,
    // you need only the first C.rows of them, and thus you
    // pass nonzeroRows == C.rows
    cv::dft(tempA, tempA, cv::DFT_INVERSE + cv::DFT_SCALE, C.rows());
    // now copy the result back to C.
    tempA(cv::Rect(0, 0, C.cols(), C.rows())).copyTo(C);

}
unsigned int K_SIZE = 0;
cv::Mat GABOR_KERNEL[3];
void saveGaborKernel(int kernel_size)
{
    const double th = CV_PI / 4;
    const double lm = CV_PI / 2;//10.0;
    const double gm = 0.5;

    int i = 0;
    for (auto sig = 5; sig <= 15; sig += 5)
    {
        GABOR_KERNEL[i] = cv::getGaborKernel(cv::Size(kernel_size, kernel_size), sig, th, lm, gm);
        i++;
    }
    K_SIZE = kernel_size;
}
// применить фильтр габора для всей картинки а а дальше окном считать мат ожидание и тд
void calculateDescriptor(const cv::Mat& image, int kernel_size, descriptor& descr)
{
    descr.clear();
    const double th = CV_PI / 4;
    const double lm = CV_PI / 2;//10.0;
    const double gm = 0.5;
    cv::Mat response;
    cv::Mat mean;
    cv::Mat dev;
    cv::Mat cloneImage;
    
    image.copyTo(cloneImage);
    cloneImage.convertTo(cloneImage, CV_64FC1);
    
    // \todo implement complete texture segmentation based on Gabor filters
    // (find good combinations for all Gabor's parameters)
    int i = 0;
    for (auto sig = 5; sig <= 15; sig += 5)
    {
        if (kernel_size != K_SIZE)
        {
            saveGaborKernel(kernel_size);
        }
        //cv::Mat kernel = cv::getGaborKernel(cv::Size(kernel_size, kernel_size), sig, th, lm, gm);
        //cv::filter2D(image, response, CV_32F, GABOR_KERNEL[i
        convolveDFT(GABOR_KERNEL[i], cloneImage, response);
        i++;
        cv::meanStdDev(response, mean, dev);
        descr.emplace_back(mean.at<double>(0));
        descr.emplace_back(dev.at<double>(0));
    }
}



} // namespace

namespace cvlib
{
cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps)
{
    cv::Mat imROI = image(roi);

    const int kernel_size = std::min(roi.height, roi.width) / 2; // \todo round to nearest odd

    descriptor reference;
    calculateDescriptor(image(roi), kernel_size, reference);

    cv::Mat res = cv::Mat::zeros(image.size(), CV_8UC1);

    descriptor test(reference.size());
    cv::Rect baseROI = roi - roi.tl();

    // \todo move ROI smoothly pixel-by-pixel
    for (int i = 0; i < image.size().width - roi.width; ++i)
    {
        for (int j = 0; j < image.size().height - roi.height; ++j)
        {
            auto curROI = baseROI + cv::Point(roi.width * i, roi.height * j);
            calculateDescriptor(image(curROI), kernel_size, test);
            // \todo implement and use norm L2
            res(curROI) = 255 * ((test - reference).norm_l1() <= eps);
            // \todo eps^2 мб
            //res(curROI) = 255 * ((test - reference).norm_l2() <= eps);
        }
    }

    return res;
}
} // namespace cvlib
