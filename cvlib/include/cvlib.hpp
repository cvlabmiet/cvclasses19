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
/// \param diffmean, in - threshold for merging regions
/// \return segmented image
cv::Mat split_and_merge(const cv::Mat& image, double stddev, double diffmean);

/// \brief Split and merge algorithm for image segmentation
/// \param image, in - input image
/// \param stddev, in - threshold to treat regions as homogeneous
/// \return segmented image
cv::Mat split_only(const cv::Mat& image, double stddev);

/// \brief Segment texuture on passed image according to sample in ROI
/// \param image, in - input image
/// \param roi, in - region with sample texture on passed image
/// \param eps, in - threshold parameter for texture's descriptor distance
/// \param roiChanged, in - flag for Gabor kernel recount
/// \return binary mask with selected texture
cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps, bool roiChanged);

/// \brief Motion Segmentation algorithm
class motion_segmentation : public cv::BackgroundSubtractor
{
public:
    /// \brief ctor
    motion_segmentation();
	
	cv::Mat getMin() const
	{
		return mMin;
	}
	cv::Mat getMax() const
	{
		return mMax;
	}
	cv::Mat getDiff() const
	{
		return mDiff;
	}

    /// \see cv::BackgroundSubtractor::apply
    void apply(cv::InputArray image, cv::OutputArray fgmask, double learningRate = -1) override;

    void setVarThreshold(const int& threshold)
    {
        mThreshold = threshold;
    }

    /// \see cv::BackgroundSubtractor::BackgroundSubtractor
    void getBackgroundImage(cv::OutputArray backgroundImage) const override
    {
        backgroundImage.assign(bg_model_);
    }

private:
    cv::Mat bg_model_;
    cv::Mat mMax;
    cv::Mat mMin;
    cv::Mat mDiff;
	cv::Mat mPrevFrame;
	double mMu;
	int mCounter;
    int mThreshold;
};
} // namespace cvlib

#endif // __CVLIB_HPP__
