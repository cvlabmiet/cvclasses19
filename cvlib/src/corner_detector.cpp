#include "cvlib.hpp"
#include <iostream>

#include <ctime>

std::vector<cv::Point> fast_offsets = {cv::Point(0, 0),  cv::Point(0, -3), cv::Point(1, -3),  cv::Point(2, -2),  cv::Point(3, -1), cv::Point(3, 0),
                                       cv::Point(3, 1),  cv::Point(2, 2),  cv::Point(1, 3),   cv::Point(0, 3),   cv::Point(-1, 3), cv::Point(-2, 2),
                                       cv::Point(-3, 1), cv::Point(-3, 0), cv::Point(-3, -1), cv::Point(-2, -2), cv::Point(-1, -3)};

namespace cvlib
{
cv::Ptr<corner_detector_fast> corner_detector_fast::create()
{
    return cv::makePtr<corner_detector_fast>();
}

void corner_detector_fast::detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray /*mask = cv::noArray()*/)
{
    keypoints.clear();
    if ((image.rows() < 7) || (image.cols() < 7))
        return;
    int t = 10;
    cv::Mat img_mat = image.getMat();

    for (int i = 3; i < image.rows() - 3; i++)
        for (int j = 3; j < image.cols() - 3; j++)
        {
            int ñompare_arr[17] = {0};
            cv::Point pix_pos = cv::Point(j, i);
            int Ip = img_mat.at<uint8_t>(pix_pos);
            int count_l = 0, count_d = 0;
            for (int k = 1; k <= 13; k += 4)
            {
                int Ipi = img_mat.at<uint8_t>(pix_pos + fast_offsets[k]);
                if (Ipi > Ip + t)
                {
                    ñompare_arr[k] = 1;
                    count_l++;
                }
                else if (Ipi < Ip - t)
                {
                    ñompare_arr[k] = -1;
                    count_d++;
                }
                else
                    ñompare_arr[k] = 0;
            }
            if ((count_l >= 3) || (count_d >= 3))
            {
                for (int k = 1; k <= 16; k++)
                {
                    if ((k == 1) || (k == 5) || (k == 9) || (k == 13))
                        continue;
                    int Ipi = img_mat.at<uint8_t>(pix_pos + fast_offsets[k]);
                    if (Ipi > Ip + t)
                        ñompare_arr[k] = 1;
                    else if (Ipi < Ip - t)
                        ñompare_arr[k] = -1;
                    else
                        ñompare_arr[k] = 0;
                }
                if (check_count_same_in_a_row(�ompare_arr + 1, 12))
                    keypoints.push_back(cv::KeyPoint(pix_pos, 4));
            }
        }
}

bool corner_detector_fast::check_count_same_in_a_row(int* arr, int thresh)
{
    int* first = arr;
    int* last = arr + 15;
    int* prev = first;
    int count = 1;
    int max_count = 1;

    while (++first != last)
    {
        if (*(prev++) == *first)
            ++count;
        else
        {
            count = 1;
        }
        if (count > max_count)
            max_count = count;
    }

    if (max_count >= thresh)
        return (true);

    return (false);
}

void corner_detector_fast::compute(cv::InputArray, std::vector<cv::KeyPoint>& keypoints, cv::OutputArray descriptors)
{
    std::srand(unsigned(std::time(0)));
    const int desc_length = 2;
    descriptors.create(static_cast<int>(keypoints.size()), desc_length, CV_32S);
    auto desc_mat = descriptors.getMat();
    desc_mat.setTo(0);

    int* ptr = reinterpret_cast<int*>(desc_mat.ptr());
    for (const auto& pt : keypoints)
    {
        for (int i = 0; i < desc_length; ++i)
        {
            *ptr = std::rand();
            ++ptr;
        }
    }
}

void corner_detector_fast::detectAndCompute(cv::InputArray, cv::InputArray, std::vector<cv::KeyPoint>&, cv::OutputArray descriptors, bool /*= false*/)
{
    // \todo implement me
}
} // namespace cvlib