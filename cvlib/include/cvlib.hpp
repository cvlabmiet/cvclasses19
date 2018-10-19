
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
    motion_segmentation()
    {
        m_frame_counter = 0;
        // m_bg_model_.zeros(cv::Size(640, 480), CV_8U);
    }

    /// \see cv::BackgroundSubtractor::apply
    void apply(cv::InputArray image, cv::OutputArray fgmask, double learningRate = -1) override;

    /// \see cv::BackgroundSubtractor::BackgroundSubtractor
    void getBackgroundImage(cv::OutputArray backgroundImage) const override
    {
        backgroundImage.assign(m_bg_model_);
    }

    void setThreshold(int tr)
    {
        m_threshold = tr;
    }

    void setAlpha(int al)
    {
        m_alpha = (double)al / 100;
    }

    private:
    cv::Mat m_bg_model_;

    // Порог
    int m_threshold;
    // Количество кадров, участвующих в вычислении фона
    int m_max_frame;
    // счетчик кадров
    int m_frame_counter;

    double m_alpha;
    //
};

/// \brief FAST corner detection algorithm
class corner_detector_fast : public cv::Feature2D
{
    public:
    /// \brief Fabrique method for creating FAST detector
    static cv::Ptr<corner_detector_fast> create();

    /// \see Feature2d::detect
    virtual void detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray mask = cv::noArray()) override;

    void setVarThreshold(int tr)
    {
        m_threshold = tr;
    }

    int getNumPoint()
    {
        return m_num_point;
    }

    private:
    int m_threshold;
    int m_num_point;

};
} // namespace cvlib

#endif // __CVLIB_HPP__
