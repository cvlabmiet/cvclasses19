/* Computer Vision Functions.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#ifndef __CVLIB_HPP__
#define __CVLIB_HPP__

#include <array>
#include <vector>
#include <opencv2/opencv.hpp>

namespace
{
	enum brightness_check_result
	{
		darker = -1,
	    similar,
		brighter
	};
}; //namespace
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

    private:
    cv::Mat bg_model_;
};

/// \brief FAST corner detection algorithm
class corner_detector_fast : public cv::Feature2D
{
    public:
    /// \brief Fabrique method for creating FAST detector
    static cv::Ptr<corner_detector_fast> create();

    /// \see Feature2d::detect
    virtual void detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray mask = cv::noArray()) override;
	void set_threshold(int thresh);

	private:
	brightness_check_result check_brightness(unsigned int circle_point_num, cv::Point center);
	bool is_keypoint(cv::Point center, unsigned int step, unsigned int num);

	const cv::Point circle_template_[16] = { cv::Point(0, -3), cv::Point(1, -3),  cv::Point(2, -2),  cv::Point(3, -1),
											 cv::Point(3, 0),  cv::Point(3, 1),   cv::Point(2, 2),   cv::Point(1, 3),
			                                 cv::Point(0, 3),  cv::Point(-1, 3),  cv::Point(-2, 2),  cv::Point(-3, 1),
			                                 cv::Point(-3, 0), cv::Point(-3, -1), cv::Point(-2, -2), cv::Point(-1, -3) };
	int threshold_;
	cv::Mat image_;
};
} // namespace cvlib

#endif // __CVLIB_HPP__
