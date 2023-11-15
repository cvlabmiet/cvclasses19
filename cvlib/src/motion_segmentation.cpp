/* Motion segmentation algorithm implementation.
 * @file
 * @date 2023-11-14
 * @author Yaroslav Murenkov
 */

#include "cvlib.hpp"

#include <iostream>

namespace cvlib
{
motion_segmentation::motion_segmentation(cv::InputArray bg_model) : bg_model_(bg_model.getMat())
{
}

void motion_segmentation::apply(cv::InputArray _image, cv::OutputArray _fgmask, double rate)
{
    cv::Mat image = _image.getMat();
    _fgmask.create(image.size(), CV_8UC3);
    cv::Mat fgmask = _fgmask.getMat();

    bg_model_ = (1 - rate) * bg_model_ + rate * image;
    cv::absdiff(image, bg_model_, fgmask);
    threshold(fgmask, fgmask, 25, 128, cv::THRESH_BINARY);
    cv::cvtColor(fgmask, fgmask, cv::COLOR_BGR2GRAY);
    _fgmask.assign(fgmask);
}
} // namespace cvlib
