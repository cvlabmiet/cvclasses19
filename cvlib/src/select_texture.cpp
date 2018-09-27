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

struct Descriptor
{
    Descriptor() // ctor
    {
        this->clear();
    }

    Descriptor operator-(const Descriptor& right) const
    {
        Descriptor temp = *this;
        for (int i = 0; i < 6; ++i)
        {
            temp.m_descriptor[i] -= right.m_descriptor[i];
        }
        return temp;
    }

    void clear()
    {
        m_descriptor[0] = 0;
        m_descriptor[1] = 0;
        m_descriptor[2] = 0;
        m_descriptor[3] = 0;
        m_descriptor[4] = 0;
        m_descriptor[5] = 0;
    }

    //double cv::norm	(	InputArray 	src1,
    //int 	normType = NORM_L2,
      //  InputArray 	mask = noArray()
       // )
    double norm_l2() const
    {
        double res = 0.0;
        for (int i = 0; i < 6; ++i)
        {
            res += m_descriptor[i] * m_descriptor[i];
        }
        return std::sqrt(res);
    }

    double m_descriptor[6];
};


void convolveDFT(cv::InputArray A, cv::InputArray B, cv::OutputArray C)
{
    //C.create(abs(A.rows() - B.rows()) + 1, abs(A.cols() - B.cols()) + 1, A.type());

    C.create(B.rows(), B.cols(), B.type());
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
    //std::cout << "KEK" << std::endl;
    // multiply the spectrums;
    // the function handles packed spectrum representations well
    cv::mulSpectrums(tempA, tempB, tempA, 0);
    // transform the product back from the frequency domain.
    // Even though all the result rows will be non-zero,
    // you need only the first C.rows of them, and thus you
    // pass nonzeroRows == C.rows
    cv::idft(tempA, tempA, cv::DFT_INVERSE + cv::DFT_REAL_OUTPUT + cv::DFT_SCALE, C.rows());
    // now copy the result back to C.
    tempA(cv::Rect(0, 0, C.cols(), C.rows())).copyTo(C);

}


unsigned GABOR_SIZE = 0;
cv::Mat gaborKernel[3];
cv::Mat filtredImage[3];

void saveGaborKernel(unsigned kernel_size)
{
    const double th = CV_PI / 4;
    const double lm = 10.0;
    const double gm = 0.5;
    const double sig[3] = { 5, 10, 15 };

    int i = 0;
    for (auto i = 0; i < 3; i++)
    {
        //gaborKernel[i].release();
        gaborKernel[i] = cv::getGaborKernel(cv::Size(kernel_size, kernel_size), sig[i], th, lm, gm);
    }
    GABOR_SIZE = kernel_size;
}


void filterImage(const cv::Mat& image)
{
    cv::Mat cloneImage;
    image.copyTo(cloneImage);
    cloneImage.convertTo(cloneImage, CV_32F);
    
    cv::Mat cloneFilterImage;
    for (auto i = 0; i < 3; i++)
    {

        //filtredImage[i].release();

        cv::filter2D(image, filtredImage[i], CV_32F, gaborKernel[i]);
        //std::cout << "Kek" << std::endl;
        //convolveDFT(gaborKernel[i], cloneImage, cloneFilterImage);
        //cloneFilterImage.copyTo(filtredImage[i]);
    }

}

void calculateDescriptorForRoi(const cv::Mat& image, /*descriptor& descr*/Descriptor& descr)
{
    descr.clear();
    cv::Mat response;

    cv::Mat mean;
    cv::Mat dev;
    cv::Mat cloneImage;
    image.copyTo(cloneImage);
    cloneImage.convertTo(cloneImage, CV_64FC1);

    for (int i = 0; i < 3; i++)
    {
        // не фильтровать, а взять от отфильтрованного
        cv::filter2D(image, response, CV_32F, gaborKernel[i]);
        //convolveDFT(gaborKernel[i], cloneImage, response);
        cv::meanStdDev(response, mean, dev);
        descr.m_descriptor[2 * i] = mean.at<double>(0);
        descr.m_descriptor[2 * i + 1] = dev.at<double>(0);
        //descr.emplace_back(mean.at<double>(0));
        //descr.emplace_back(dev.at<double>(0));
    }
}



void calculateDescriptor(const cv::Rect& roi, /*descriptor& descr*/Descriptor& descr)
{
    descr.clear();
    //const double th = CV_PI / 4;
    //const double lm = 10.0;
    //const double gm = 0.5;
    //cv::Mat response;
    cv::Mat mean;
    cv::Mat dev;

    // \todo implement complete texture segmentation based on Gabor filters
    // (find good combinations for all Gabor's parameters)
    //for (auto sig = 5; sig <= 15; sig += 5)
    for (auto i = 0; i < 3; i++)
    {
        cv::meanStdDev(filtredImage[i](roi), mean, dev);
        descr.m_descriptor[2 * i] = mean.at<double>(0);
        descr.m_descriptor[2 * i + 1] = dev.at<double>(0);
        //descr.emplace_back(mean.at<double>(0));
        //descr.emplace_back(dev.at<double>(0));
    }
}

} // namespace

namespace cvlib
{
cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps)
{
    cv::Mat imROI = image(roi);

    const int kernel_size = int(std::min(roi.height, roi.width) / 4) * 2 + 1;
    //const int kernel_size = std::min(roi.height, roi.width) / 2; // \todo round to nearest odd
    
    if (kernel_size != GABOR_SIZE)
    {
        saveGaborKernel(kernel_size);
    }

    //descriptor reference;
    Descriptor reference;
    //calculateDescriptor(image(roi), kernel_size, reference);
    calculateDescriptorForRoi(image(roi), reference);

    cv::Mat res = cv::Mat::zeros(image.size(), CV_8UC1);

    //descriptor test(reference.size());
    Descriptor test;
    cv::Rect baseROI = roi - roi.tl();

    //std::cout << "SIZE IMAGE" << image.size() << std::endl;
    filterImage(image);
    //std::cout << "SIZE FILTER IMAGE" << filtredImage[0].size() << std::endl;
    //std::cout << "SIZE FILTER IMAGE" << filtredImage[1].size() << std::endl;
    //std::cout << "SIZE FILTER IMAGE" << filtredImage[2].size() << std::endl;
    
    cv::Mat mask = cv::Mat(image.size(), CV_8UC1);

    // \todo move ROI smoothly pixel-by-pixel
    for (int i = 0; i < image.size().width - roi.width; ++i)
    {
        for (int j = 0; j < image.size().height - roi.height; ++j)
        {
            auto curROI = baseROI + cv::Point( i, j);
            
            //if (cv::countNonZero(mask(curROI)) != kernel_size* kernel_size)
            //{

                calculateDescriptor(curROI, test);
                int a = 255 * ((test - reference).norm_l2() <= eps);
                //res(curROI) = 255 * ((test - reference).norm_l2() <= eps);
                if (a != 0)
                {
                    res(curROI) = 255 * a;
                    mask(curROI) = 1;
                    j += kernel_size;

                }
            //}
            
        }
    }

    return res;
}
} // namespace cvlib

/*
namespace cvlib
{
    cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps)
    {
        const int kernel_size = 13;//int(std::min(roi.height, roi.width) / 4) * 2 + 1;

        const double th = CV_PI / 4;
        const double lm = 10.0;
        const double gm = 0.5;
        const double sig[3] = { 5, 10, 15 };
        

        cv::Mat response = cv::Mat::zeros(image.size(), CV_64FC(1));

        cv::Mat cloneImage;
        image.copyTo(cloneImage);
        cloneImage.convertTo(cloneImage, CV_64FC1);

        cv::Mat kernel = cv::getGaborKernel(cv::Size(kernel_size, kernel_size), sig[0], th, lm, gm);


        //cv::filter2D(image, response, CV_32F, kernel);

        convolveDFT(kernel, cloneImage, response);
        //std::cout << "Size conv = " << response << std::endl;
        return response;
    }
} // namespace cvlib

*/

/*
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
        //cv::filter2D(image, response, CV_32F, GABOR_KERNEL[i]);
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
*/