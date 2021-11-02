/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Anonymous
 */

#include "cvlib.hpp"

#include <iostream>

namespace cvlib
{
motion_segmentation::motion_segmentation()
{
    history.reserve(history_cap);
}

void motion_segmentation::apply(cv::InputArray _image, cv::OutputArray _fgmask, double)
{
    // \todo implement your own algorithm:
    //       * MinMax
    //       * Mean
    //       * 1G
    //       * GMM

    // \todo implement bg model updates
    cv::Mat input;
    cv::cvtColor(_image, input, cv::COLOR_BGR2GRAY);
    input.convertTo(input, CV_32FC1);
    
    if (method == BSmethod::Mean)
    {
        if (history.size() < history_cap)
        {
            history.push_back(input);
            if (frame_counter == 0)
                input.copyTo(bg_model_);
            else
                bg_model_ += input.mul(1 / 10);
            frame_counter++;
        }
        else
        {
            cv::Mat absdiff;
            cv::absdiff(bg_model_, input, absdiff);
            cv::threshold(absdiff, _fgmask, threshold_m, 255, CV_8U);
            updateBackgroundModel(input);
        }
    }else if(method == BSmethod::G1)
    {
        if(frame_counter == 0)
        {
            input.copyTo(mu);
            sigma = cv::Mat::ones(input.size(), CV_32FC1)*20;
            frame_counter++;
        }else
        {;
            cv::Mat absdiff;
            cv::absdiff(mu, input, absdiff);
            cv::divide(absdiff, sigma, absdiff);
            cv::threshold(absdiff, _fgmask, threshold_g, 255, CV_8U);
            updateBackgroundModel(input, _fgmask.getMat());
        }
    }
}

void motion_segmentation::updateBackgroundModel(cv::Mat image, cv::Mat mask)
{
    if (method == BSmethod::Mean)
    {
        //bg_model_ *= (1 - background_alpha);
        //image *= background_alpha;
        //bg_model_ += image;
        frame_counter++;
        frame_counter %= history_cap;
        image.copyTo(history[frame_counter]);
        bg_model_.setTo(0);
        for(const auto& h : history)
            bg_model_ += h / history_cap;

    }else if(method == BSmethod::G1)
    {
        cv::Mat absdiff;
        cv::absdiff(image, mu, absdiff);
        for(int i = 0; i < mu.rows; i++)
            for(int j = 0; j < mu.cols; j++)
            {
                if(mask.at<uint8_t>(i,j) != 255)
                {
                    mu.at<float>(i,j) = (1-background_alpha) * mu.at<float>(i,j) + background_alpha * image.at<float>(i,j);
                
                    sigma.at<float>(i,j) = std::sqrt((1-background_alpha) * sigma.at<float>(i,j) * sigma.at<float>(i,j)
                                                 + background_alpha * absdiff.at<float>(i,j) * absdiff.at<float>(i,j));
                }
            }
    }
}

void motion_segmentation::setMethod(BSmethod m)
{
    if(method == m) return;

    method = m;
    frame_counter = 0;
    history.clear();
    std::cout<< frame_counter << std::endl;
}

} // namespace cvlib
