/* FAST corner detector algorithm implementation.
 * @file
 * @date 2018-10-16
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <ctime>

#include <random>
#include <vector>
#include <algorithm>

namespace cvlib
{
// static
cv::Ptr<corner_detector_fast> corner_detector_fast::create()
{
    return cv::makePtr<corner_detector_fast>();
}

    cv::Point generate_point(int sigma)
{
    // std::default_random_engine generator;
    std::random_device rd{};
    std::mt19937 generator{rd()};

    std::normal_distribution<float> distribution_x(0, sigma);
    int x = std::round(distribution_x(generator));

    std::normal_distribution<float> distribution_y(x, sigma);
    int y = std::round(distribution_y(generator));

    return {std::clamp(x, -sigma, sigma), std::clamp(y, -sigma, sigma)};
}


void make_point_pairs(std::vector<std::pair<cv::Point, cv::Point>>& pairs, const int desc_length, const int neighbourhood_size)
{
    pairs.clear();
    const int halfsize = neighbourhood_size / 2;
    for (int i = 0; i < desc_length; i++)
        pairs.push_back(std::make_pair(generate_point(halfsize), generate_point(halfsize)));
}



void corner_detector_fast::detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints)
{
    keypoints.clear();
    cv::Mat img = image.getMat();
    if (img.channels() > 1) cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);

    const int threshold = 30, t = 9;
    std::vector<cv::Point> offsets = {cv::Point(0, 3),  cv::Point(1, 2),   cv::Point(2, 2),   cv::Point(3, 1),
                                      cv::Point(3, 0),  cv::Point(3, -1),  cv::Point(2, -2),  cv::Point(1, -3),
                                      cv::Point(0, -3), cv::Point(-1, -3), cv::Point(-2, -2), cv::Point(-3, -1),
                                      cv::Point(-3, 0), cv::Point(-3, 1),  cv::Point(-2, 2),  cv::Point(-1, 3)};
    for (int y = 3; y < img.rows - 3; y++) 
    {
        for (int x = 3; x < img.cols - 3; x++) 
        {
            int centerPixel = img.at<uchar>(y, x);
            std::vector<int> binaryCircle(16, 0);
            for (int i = 0; i < binaryCircle.size(); i++)
            {
                int areaPixel = img.at<uchar>(y + offsets[i].y, x + offsets[i].x);
                if (areaPixel > centerPixel + threshold)
                    binaryCircle[i] = 1;
                else if (areaPixel < centerPixel - threshold)
                    binaryCircle[i] = 2;
            }
            int count = 0;
            int currentValue = binaryCircle[0];
            for (int i = 0; i < (16 + t - 1); i++) 
            {
                if (currentValue == 0)
                {
                    count = 0;
                    currentValue = binaryCircle[(i + 1) % 16];
                    continue;
                }
                if (binaryCircle[i % 16] == currentValue)
                {
                    count++;
                    if (count >= t)
                    {
                        keypoints.emplace_back(cv::KeyPoint(cv::Point2f(x, y), 7));
                        break;
                    }
                }
                else
                {
                    currentValue = binaryCircle[i % 16];
                    count = 1;
                }
            }
        }
    }
}

void corner_detector_fast::compute(cv::InputArray image, std::vector<cv::KeyPoint>& keypoints, cv::OutputArray descriptors)
{
    const int desc_length = 2;

    cv::Mat img;
    image.getMat().copyTo(img);
    if (img.channels() == 3)
        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

    std::vector<std::pair<cv::Point, cv::Point>> pairs;
    const int neighbourhood_size = 25;
    descriptors.create(static_cast<int>(keypoints.size()), desc_length, CV_8U); //  CV_8U
    auto desc_mat = descriptors.getMat();
    desc_mat.setTo(0);

    make_point_pairs(pairs, desc_length, neighbourhood_size);
    auto test = [&img](cv::Point kp, std::pair<cv::Point, cv::Point> p) { return img.at<uint8_t>(kp + p.first) < img.at<uint8_t>(kp + p.second); };
    auto ptr = reinterpret_cast<uint8_t*>(desc_mat.ptr());
    for (const auto& kp : keypoints)
    {
        for (int i = 0; i < desc_length; ++i)
        {
            uint8_t descriptor = 0;
            for (auto j = 0; j < pairs.size(); ++j)
                descriptor |= (test(kp.pt, pairs.at(j)) << (pairs.size() - 1 - j));
            *ptr = descriptor;
            ++ptr;
        }
    }
}

void corner_detector_fast::detectAndCompute(cv::InputArray img, 
                                            cv::InputArray mask, std::vector<cv::KeyPoint>& keypoints,
                                            cv::OutputArray descriptors,
                                            bool useKeyPoints)
{
    if (!useKeyPoints) detect(img, keypoints);
    compute(img, keypoints, descriptors);
}
} // namespace cvlib
