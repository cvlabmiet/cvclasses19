/* Computer Vision Functions implementation.
 * @file
 * @date 2018-09-18
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <iostream>

namespace cvlib
{
void motion_segmentation::apply(cv::InputArray _image, cv::OutputArray _fgmask, double learning_rate)
{
    cv::Mat image;
    _image.getMat().convertTo(image, CV_32FC3);

    const double lr = (learning_rate < 0.0) ? alpha_ : learning_rate;

    if (bg_model_.empty())
    {
        image.copyTo(bg_model_);
        _fgmask.create(image.size(), CV_8U);
        _fgmask.setTo(0);
        return;
    }

    cv::Mat diff;
    cv::absdiff(image, bg_model_, diff);

    cv::Mat gray;
    cv::cvtColor(diff, gray, cv::COLOR_BGR2GRAY);

    cv::Mat mask = gray > diff_threshold_;
    _fgmask.assign(mask);

    bg_model_ = (1.0 - lr) * bg_model_ + lr * image;
}
}