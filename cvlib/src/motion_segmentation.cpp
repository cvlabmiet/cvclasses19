/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Anonymous
 */

#include "cvlib.hpp"

#include <iostream>

namespace cvlib
{
 void motion_segmentation::getBackgroundImage(cv::Mat& bg_model)
    {
        bg_model = cv::Mat::zeros(memmory[0].size(), memmory[0].type());
        for (int i = 0; i < bg_model.rows; i++)
        {
            for (int j = 0; j < bg_model.cols; j++)
            {
                int val = 0;
                for (auto n = 0; n < memmory.size(); n++)
                {
                    val += memmory[n].at<unsigned char>(i, j);
                }
                bg_model.at<unsigned char>(i, j) = val / memmory.size();
            }
        }
    }
void motion_segmentation::apply(cv::InputArray _image, cv::OutputArray _fgmask, double)
{
    if(meanlogic)
    {
        if (this->memmory.size() < max_size)
        {
            memmory.push_back(_image.getMat());
        }
        else
        {
            memmory.erase(memmory.begin());
            memmory.push_back(_image.getMat());
        }
        cv::Mat absdiff_image, background;
        getBackgroundImage(background);
        cv::absdiff(memmory[0], background, absdiff_image);
        cv::threshold(absdiff_image, _fgmask, thresh, 255, 0);
    }
    else
    {
        auto image = _image.getMat().clone();
        image.convertTo(image,CV_64FC1);
        if(m_value.empty())
        {
            m_value = image.clone();
            d_value = cv::Mat::ones(m_value.size(),CV_64FC1);
        }
        updateBackground(image);
        cv::Mat absdiff_image;
        cv::absdiff(m_value, image, absdiff_image);

        cv::divide(absdiff_image,d_value,absdiff_image);
        cv::threshold(absdiff_image, _fgmask, k, 255.0, 0);
    }

}
void motion_segmentation::updateBackground(cv::Mat _image)
{
    cv::Mat absdiff_image;
    cv::absdiff(m_value, _image, absdiff_image);

    absdiff_image.convertTo(absdiff_image,CV_64FC1);
    for (int i = 0; i < m_value.rows; i++)
    {
        for (int j = 0; j < m_value.cols; j++)
        {
            m_value.at<double>(i, j) = m_value.at<double>(i, j)*(1-p)+p*_image.at<double>(i,j);
            d_value.at<double>(i, j) = sqrt(d_value.at<double>(i, j)*d_value.at<double>(i, j)*(1-p)+absdiff_image.at<double>(i,j)*absdiff_image.at<double>(i,j)*p);
        }
    }

}
void motion_segmentation::setVarThreshold(double varThreshold)
{
    thresh = varThreshold;
    k = varThreshold/50;
}
} // namespace cvlib
