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
    // \todo implement your own algorithm:
    //       * MinMax
    //       * Mean
    //       * 1G
    //       * GMM

    // \todo implement bg model updates
}
} // namespace cvlib
