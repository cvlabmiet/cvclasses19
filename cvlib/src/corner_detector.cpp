/* FAST corner detector algorithm implementation.
 * @file
 * @date 2018-10-16
 * @author Anonymous
 */

#include "cvlib.hpp"

#include <cmath>
#include <ctime>
namespace cvlib
{
// static

auto generate_pairs(int pairs_num, int S = 32, int seed = 3)
{
    srand(seed);
    std::vector<std::pair<int, int>> pairs;
    for (int i = 0; i < 2 * pairs_num; ++i)
    {
        int x = rand() % S - S / 2;
        int y = rand() % S - S / 2;
        pairs.push_back(std::make_pair(x, y));
    }
    return pairs;
}

std::vector<int> dx = {0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2, -1};
std::vector<int> dy = {3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2, -1, 0, 1, 2, 3};
std::vector<int> fast_ind = {0, 4, 8, 12};
std::vector<int> hard_ind = {1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15};
std::pair<int, bool> fastcheck(cv::Mat& image, int x, int y, int thresh = 10)
{
    int Ipt = image.at<unsigned char>(x, y) + thresh;
    int Imt = image.at<unsigned char>(x, y) - thresh;
    int pos_count = 0, neg_count = 0;
    for (int i = 0; i < fast_ind.size(); ++i)
    {
        int cur_dx = dx[fast_ind[i]];
        int cur_dy = dy[fast_ind[i]];
        int cur_x = x - cur_dx;
        int cur_y = y - cur_dy;
        if (cur_x >= 0 && cur_y >= 0)
        {
            if (image.at<unsigned char>(cur_x, cur_y) > Ipt)
            {
                pos_count++;
            }
            if (image.at<unsigned char>(cur_x, cur_y) < Imt)
            {
                neg_count++;
            }
        }
    }
    if (pos_count >= 3)
        return std::make_pair(pos_count, true);
    if (neg_count >= 3)
        return std::make_pair(neg_count, false);
    return std::make_pair(-1, false);
}
bool hardcheck(cv::Mat& image, int x, int y, int thresh)
{
    auto check = fastcheck(image, x, y);
    if (check.first == -1)
        return false;
    int num = check.first;
    int Ipt = image.at<unsigned char>(x, y) + thresh;
    int Imt = image.at<unsigned char>(x, y) - thresh;

    for (int i = 0; i < hard_ind.size(); i++)
    {
        int cur_dx = dx[hard_ind[i]];
        int cur_dy = dy[hard_ind[i]];
        int cur_x = x - cur_dx;
        int cur_y = y - cur_dy;
        if (cur_x >= 0 && cur_y >= 0)
        {
            if (check.first)
            {
                if (image.at<unsigned char>(cur_x, cur_y) > Ipt)
                {
                    num++;
                }
            }
            else
            {
                if (image.at<unsigned char>(cur_x, cur_y) < Imt)
                {
                    num++;
                }
            }
        }
    }
    if (num >= 12)
        return true;
    else
        return false;
}
cv::Ptr<corner_detector_fast> corner_detector_fast::create()
{
    return cv::makePtr<corner_detector_fast>();
}

void corner_detector_fast::detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray /*mask = cv::noArray()*/)
{
    keypoints.clear();
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    for (int i = 0; i < gray.rows; i++)
    {
        for (int j = 0; j < gray.cols; j++)
        {
            if (hardcheck(gray, i, j, this->thresh))
            {
                keypoints.push_back(cv::KeyPoint(j, i, 3 * 2 + 1, 0, 0, 0, 3));
            }
        }
    }

    // \todo implement FAST with minimal LOCs(lines of code), but keep code readable.
}

void corner_detector_fast::compute(cv::InputArray image, std::vector<cv::KeyPoint>& keypoints, cv::OutputArray descriptors)
{
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    // std::srand(unsigned(std::time(0))); // \todo remove me
    // \todo implement any binary descriptor
    const int desc_length = 256;
    const int S = 32;
    std::vector<cv::KeyPoint> _keypoints;
    for (int i = 0; i < keypoints.size(); i++)
    {
        auto point = keypoints[i];
        if (point.pt.x - S / 2 >= 0 && point.pt.x + S / 2 < gray.cols && point.pt.y - S / 2 >= 0 && point.pt.y + S / 2 < gray.rows)
        {
            _keypoints.push_back(point);
        }
    }
    std::cout << "old_size:" << keypoints.size()<<" new_size:"<< _keypoints.size() << std::endl;
    descriptors.create(static_cast<int>(_keypoints.size()), desc_length, CV_32S);

    GaussianBlur(gray, gray, cv::Size(5, 5), 0, 0);

    auto pairs = generate_pairs(desc_length * 2, S);
    auto desc_mat = descriptors.getMat();
    desc_mat.setTo(0);

    for (int i = 0; i < desc_mat.rows; i++)
    {
        auto cur_point = _keypoints[i];
        for (int j = 0; j < desc_mat.cols; j++)
        {
            //std::cout << i << " " << j << std::endl;
            int x = gray.at<unsigned char>(cur_point.pt.x + pairs[j * 2].first, cur_point.pt.y + pairs[j * 2].second);
            int y = gray.at<unsigned char>(cur_point.pt.x + pairs[j * 2 + 1].first, cur_point.pt.y + pairs[j * 2 + 1].second);
            if (x < y)
                desc_mat.at<int>(i, j) = 1;
            else
                desc_mat.at<int>(i, j) = 0;
        }
    }

    //    descriptors.create(static_cast<int>(keypoints.size()), desc_length, CV_32S);
    //    auto desc_mat = descriptors.getMat();
    //    desc_mat.setTo(0);
    //
    //    int* ptr = reinterpret_cast<int*>(desc_mat.ptr());
    //    for (const auto& pt : keypoints)
    //    {
    //        for (int i = 0; i < desc_length; ++i)
    //        {
    //            *ptr = std::rand();
    //            ++ptr;
    //        }
    //    }
}

void corner_detector_fast::detectAndCompute(cv::InputArray image, cv::InputArray masc, std::vector<cv::KeyPoint>& corners, cv::OutputArray descriptors, bool /*= false*/)
{
    this->detect(image,corners);
    this->compute(image, corners,descriptors);
    // \todo implement me
}
} // namespace cvlib
