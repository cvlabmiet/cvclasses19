/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Anonymous
 */

#include "cvlib.hpp"

#include <iostream>

namespace cvlib
{
void motion_segmentation::apply(cv::InputArray _image, cv::OutputArray _fgmask, double)
{
    // implement your own algorithm: Mean

    cv::Mat img;
    _image.copyTo(img);

    if (counter == 0) 
		bg_model_ = img;

    if (counter < max_frame)
    {
        counter++;
        bg_model_ += (img - bg_model_) / max_frame;
    }
    else
    {
        bg_model_ = (1 - alpha) * bg_model_ + alpha * img;
    }

    cv::threshold(abs(img - bg_model_), _fgmask, threshold, 255, cv::THRESH_BINARY);
    
	int erosion_size = 2;
    cv::Mat Kern =
        cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1));
    cv::erode(_fgmask, _fgmask, Kern);
	cv::dilate(_fgmask, _fgmask, Kern);
}
} // namespace cvlib
