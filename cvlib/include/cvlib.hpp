/* Computer Vision Functions.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#ifndef __CVLIB_HPP__
#define __CVLIB_HPP__

#include <opencv2/opencv.hpp>

namespace cvlib
{
/// \brief Split and merge algorithm for image segmentation
/// \param image, in - input image
/// \param stddev, in - threshold to treat regions as homogeneous
/// \return segmented image
cv::Mat split_and_merge(const cv::Mat& image, double stddev);

/// \brief Segment texuture on passed image according to sample in ROI
/// \param image, in - input image
/// \param roi, in - region with sample texture on passed image
/// \param eps, in - threshold parameter for texture's descriptor distance
/// \return binary mask with selected texture
cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps);

/// \brief Motion Segmentation algorithm
class motion_segmentation : public cv::BackgroundSubtractor
{
    public:
    /// \brief ctor
	motion_segmentation(unsigned int frames_for_init):is_initialized_(false),
		frames_for_init_(frames_for_init), current_frame_(0) {};

    /// \see cv::BackgroundSubtractor::apply
    void apply(cv::InputArray image, cv::OutputArray fgmask, double learningRate = 0.05) override;

    /// \see cv::BackgroundSubtractor::BackgroundSubtractor
    void getBackgroundImage(cv::OutputArray backgroundImage) const override
    {
        backgroundImage.assign(bg_model_);
    }
	/// \brief check for background initialization
	bool isinitialized() const
	{
		return is_initialized_;
	}

	/// \brief set variance threshold
	void setVarThreshold(double threshold);

    private:
    cv::Mat bg_model_;
	cv::Mat variance_;
	double var_threshold_;
	unsigned int frames_for_init_;
	unsigned int current_frame_;
	bool is_initialized_;
};
} // namespace cvlib

#endif // __CVLIB_HPP__
