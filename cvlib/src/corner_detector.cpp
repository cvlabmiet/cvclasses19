/* FAST corner detector algorithm implementation.
 * @file
 * @date 2018-10-16
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>

namespace cvlib
{
// static
cv::Ptr<corner_detector_fast> corner_detector_fast::create()
{
    return cv::makePtr<corner_detector_fast>();
}

void corner_detector_fast::detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray /*mask = cv::noArray()*/)
{
    keypoints.clear();
    // \todo implement FAST with minimal LOCs(lines of code), but keep code readable.

    //cv::Mat image;
    //
    cv::Size image_size = image.size();
    
    //std::cout << image_size << std::endl;
    
    cv::Mat imag = image.getMat();


    std::vector<int> circle_points(16);
    std::vector<int> target;
    int i_c = 0;
    int i_plus_thresh = 0;
    int i_minus_thresh = 0;

    m_threshold = 1;
    for (auto i = 3; i < image_size.height - 3; i++)
    {
        for (auto j = 3; j < image_size.width - 3; j++)
        {
            i_c = imag.at<int>(i,j);
            i_plus_thresh = i_c + m_threshold;
            i_minus_thresh = i_c - m_threshold;

            circle_points.push_back(imag.at<int>(i - 3, j)); // 1
            circle_points.push_back(imag.at<int>(i - 3, j + 1)); // 2
            circle_points.push_back(imag.at<int>(i - 2, j  + 2)); // 3
            circle_points.push_back(imag.at<int>(i - 1, j + 3)); // 4
            circle_points.push_back(imag.at<int>(i, j + 3)); // 5
            circle_points.push_back(imag.at<int>(i + 1, j + 3)); // 6
            circle_points.push_back(imag.at<int>(i + 2, j + 2)); // 7
            circle_points.push_back(imag.at<int>(i + 3, j + 1)); // 8
            circle_points.push_back(imag.at<int>(i + 3, j)); // 9
            circle_points.push_back(imag.at<int>(i + 3, j -1)); // 10
            circle_points.push_back(imag.at<int>(i + 2, j -2)); // 11
            circle_points.push_back(imag.at<int>(i + 1, j -3)); // 12
            circle_points.push_back(imag.at<int>(i, j -3)); // 13
            circle_points.push_back(imag.at<int>(i -1, j - 3)); // 14
            circle_points.push_back(imag.at<int>(i - 2, j - 2)); // 15
            circle_points.push_back(imag.at<int>(i - 3, j - 1)); // 16

            //for (auto l = circle_points.begin(); l != circle_points.end(); l++)
            //    std::cout << *l << std::endl;

            //std::cout << "Imag " << image.at<int>(i - 3, j) << "\n Vector" << circle_points.at(0) << std::endl;
            if ((circle_points.at(0) > i_plus_thresh && circle_points.at(4) > i_plus_thresh && circle_points.at(8) > i_plus_thresh && circle_points.at(12) > i_plus_thresh) || 
                (circle_points.at(0) < i_minus_thresh && circle_points.at(4) < i_minus_thresh && circle_points.at(8) < i_minus_thresh && circle_points.at(12) < i_minus_thresh))
            {
                std::copy_if(circle_points.begin(), circle_points.end(), std::back_inserter(target), [=](int n) { return n < i_minus_thresh; });

                if (target.size() >= 12)
                {
                    keypoints.push_back(cv::KeyPoint(cv::Point2f(double(4*j), double(i)), 10, float(0)));
                }

                target.clear();
                std::copy_if(circle_points.begin(), circle_points.end(), std::back_inserter(target), [=](int n) { return n > i_plus_thresh; });

                if (target.size() >= 12)
                    keypoints.push_back(cv::KeyPoint(cv::Point2f(double(4*j), double(i)), float(0)));

            }
                            
            circle_points.clear();
            target.clear();
        }

    }
    /*keypoints.push_back(cv::KeyPoint(cv::Point2f(double(10), double(10)), 10));
    keypoints.push_back(cv::KeyPoint(cv::Point2f(double(100), double(100)), 10));
    keypoints.push_back(cv::KeyPoint(cv::Point2f(double(200), double(200)), 10));
    keypoints.push_back(cv::KeyPoint(cv::Point2f(double(300), double(300)), 10));
    keypoints.push_back(cv::KeyPoint(cv::Point2f(double(400), double(400)), 10));
    keypoints.push_back(cv::KeyPoint(cv::Point2f(double(420), double(470)), 10));
    keypoints.push_back(cv::KeyPoint(cv::Point2f(double(500), double(470)), 10));*/
    //keypoints.push_back(cv::KeyPoint(cv::Point2f(100.0, 200.0), 10));
}
} // namespace cvlib
