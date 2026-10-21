/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Anonymous
 */

#include "cvlib.hpp"

#include <iostream>

namespace cvlib
{

    motion_segmentation::motion_segmentation()
{
}
void motion_segmentation::apply(cv::InputArray _image, cv::OutputArray _fgmask, double lr)
{
    cv::Mat image = _image.getMat();

    if (bg_model_.empty())
    {
        image.convertTo(bg_model_, CV_32F);
        _fgmask.assign(cv::Mat::zeros(image.size(), CV_8UC1));
        return;
    }

    int th = (lr > 0) ? static_cast<int>(lr) : 25;
    cv::Mat image_f;
    image.convertTo(image_f, CV_32F);
    cv::Mat diff;
    cv::absdiff(image_f, bg_model_, diff);
    cv::threshold(diff, _fgmask, th, 255, cv::THRESH_BINARY);

    cv::accumulateWeighted(image, bg_model_, 0.05);
}
} // namespace cvlib
