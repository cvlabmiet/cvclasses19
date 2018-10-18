/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Dmitrij Vukalov
 */

#include "cvlib.hpp"

#include <iostream>

const int max_frame = 100;

namespace cvlib
{
void motion_segmentation::apply(cv::InputArray _image, cv::OutputArray _fgmask, double)
{
    cv::Size sz = _image.size();
    cv::Mat img;
    _image.copyTo(img);

    if (m_frame_counter > max_frame || m_frame_counter == 0)
    {
        m_frame_counter = 0;
        m_frame_counter++;
        m_bg_model_ = img;
    }
    else
    {
        m_frame_counter++;
        m_bg_model_ += (img - m_bg_model_) / m_frame_counter;
        m_bg_model_ = (1 - m_alpha) * m_bg_model_ + m_alpha * img;
    }

    cv::threshold(abs(img - m_bg_model_), _fgmask, m_threshold, 255, cv::THRESH_BINARY);
}
} // namespace cvlib
