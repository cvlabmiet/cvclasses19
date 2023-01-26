#include "cvlib.hpp"
#include <iostream>

namespace cvlib
{
motion_segmentation::motion_segmentation() : mMu(0), mCounter(0)
{
}
void motion_segmentation::apply(cv::InputArray _image, cv::OutputArray _fgmask, double)
{
    if (mCounter > 100)
        mCounter = 0;

    if (_image.empty())
        return;
    cv::Mat image = _image.getMat().clone();
    cv::GaussianBlur(image, image, cv::Size(5, 5), 0);
    cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

    if (!mCounter)
    {
        image.copyTo(mMax);
        image.copyTo(mMin);
        mDiff = cv::Mat::zeros(image.size(), CV_8UC1);
        mPrevFrame = cv::Mat::zeros(image.size(), CV_8UC1);
    }

    mMu = 0;
    cv::Mat fgmask = cv::Mat::zeros(image.size(), CV_8UC1);

    for (int i = 0; i < image.size().width; i++)
    {
        for (int j = 0; j < image.size().height; j++)
        {
            uchar pix = image.at<uchar>(j, i);
            mMax.at<uchar>(j, i) = std::max(mMax.at<uchar>(j, i), pix);
            mMin.at<uchar>(j, i) = std::min(mMin.at<uchar>(j, i), pix);

            if (mCounter > 0)
            {
                uchar diff = (uchar)std::abs(double(pix) - double(mPrevFrame.at<uchar>(j, i)));
                mDiff.at<uchar>(j, i) = std::max(mDiff.at<uchar>(j, i), diff);
            }

            mPrevFrame.at<uchar>(j, i) = pix;
        }
    }

    cv::Mat tmp;
    cv::sort(mDiff, tmp, cv::SORT_EVERY_ROW + cv::SORT_ASCENDING);
    mMu = tmp.at<uchar>(tmp.size().height / 2, tmp.size().width / 2);

    for (int i = 0; i < image.size().width; i++)
    {
        for (int j = 0; j < image.size().height; j++)
        {
            double pix = (double)image.at<uchar>(j, i);
            bool cond1 = std::abs((double)mMax.at<uchar>(j, i) - pix) > (double)mThreshold / 100 * mMu;
            bool cond2 = std::abs((double)mMin.at<uchar>(j, i) - pix) > (double)mThreshold / 100 * mMu;
            fgmask.at<uchar>(j, i) = (cond1 || cond2) * 255;
        }
    }

    cv::Mat mask;
    cv::bitwise_not(fgmask, mask);
    cv::bitwise_or(_image, _image, bg_model_, mask);

    fgmask.copyTo(_fgmask);
    mCounter++;
}
} // namespace cvlib