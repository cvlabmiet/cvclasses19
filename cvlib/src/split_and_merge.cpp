/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <cstring>
#include <cstdlib>
#include <vector>

namespace
{
std::vector<int> gIndeces;
void area_by_index(int index, cv::Mat &image, cv::Mat &roi)
{
    const auto width = image.cols;
    const auto height = image.rows;

    std::string str = std::to_string(index);

    int w = int(double(width) / pow(2, int(strlen(str.c_str()))));
    int h = int(double(height) / pow(2, int(strlen(str.c_str()))));

    int x = 0;
    int y = 0;
    int step_w = width / 2;
    int step_h = height / 2;

    for (std::string::size_type i = 0; i < str.size(); i++)
    {
        int sym = static_cast<int>(str.c_str()[i]);
        if (sym == 2)
            x += step_w;
        else if (sym == 3)
            y += step_h;
        else if (sym == 4)
        {
            x += step_w;
            y += step_h;
        }
        step_w /= 2;
        step_h /= 2;
    }

    roi = image(cv::Range(y, y + h), cv::Range(x, x + w)).clone();
}

bool neighbour_indeces(int index1, int index2)
{
    std::string str1 = std::to_string(index1);
    std::string str2 = std::to_string(index2);
    std::string str;
    int len1 = (int)strlen(str1.c_str());
    int len2 = (int)strlen(str2.c_str());

    bool same_areas = true;
    int dir = 0; 

    for (int i = 0; i < std::min(len1, len2); i++)
    {
        int j1 = static_cast<int>(str1.c_str()[i]);
        int j2 = static_cast<int>(str2.c_str()[i]);

        if (same_areas)
            if (j1 == j2)
                continue;
            else 
            {
                if (j1 == 1 && j2 == 2)
                    dir = 1; 
                if (j1 == 3 && j2 == 4)
                    dir = 1; 
                if (j1 == 1 && j2 == 3)
                    dir = 2;
                if (j1 == 2 && j2 == 4)
                    dir = 2;
                if (j1 == 2 && j2 == 1)
                    dir = 3;
                if (j1 == 4 && j2 == 3)
                    dir = 3;
                if (j1 == 3 && j2 == 1)
                    dir = 4;
                if (j1 == 4 && j2 == 2)
                    dir = 4;
            }

        same_areas = false;
        switch (dir)
        {
            case 0:
                return false;
            case 1: {
                if (!(j1 == 2 && j2 == 1) || !(j1 == 4 && j2 == 3))
                    return false;
            }
            case 2: {
                if (!(j1 == 4 && j2 == 2) || !(j1 == 3 && j2 == 1))
                    return false;
            }
            case 3: {
                if (!(j1 == 1 && j2 == 2) || !(j1 == 3 && j2 == 4))
                    return false;
            }
            case 4: {
                if (!(j1 == 2 && j2 == 4) || !(j1 == 1 && j2 == 3))
                    return false;
            }
        }

        
    }
    if (len1 > len2)
        std::string str = str1;
    else
        std::string str = str2;

    for (int i = std::min(len1, len2); i < std::max(len1, len2); i++)
    {
        int j = static_cast<int>(str.c_str()[i]);
        switch (dir)
        {
            case 1: {
                if (j != 1 || j != 3)
                    return false;
            }
            case 2: {
                if (j != 1 || j != 2)
                    return false;
            }
            case 3: {
                if (j != 2 || j != 4)
                    return false;
            }
            case 4: {
                if (j != 3 || j != 4)
                    return false;
            }
        }
    }
    return true;
}

void split_image(cv::Mat image, double stddev, int current_index)
{
    const auto width = image.cols;
    const auto height = image.rows;

    if (image.empty())
        return; 
    
    cv::Mat mean;
    cv::Mat dev;
    cv::meanStdDev(image, mean, dev);

    if (dev.at<double>(0) <= stddev || width < 12 || height < 12)
    {
        image.setTo(mean);
        gIndeces.push_back(current_index);
        return;
    }

    split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev, 10 * current_index + 1);
    split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev, 10 * current_index + 2);
    split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev, 10 * current_index + 4);
    split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev, 10 * current_index + 3);
}

void set_to_by_index(cv::Mat image, int index, cv::Mat color)
{
    const auto width = image.cols;
    const auto height = image.rows;

    std::string str = std::to_string(index);

    int w = int(double(width) / pow(2, int(strlen(str.c_str()))));
    int h = int(double(height) / pow(2, int(strlen(str.c_str()))));

    int x = 0;
    int y = 0;
    int step_w = width / 2;
    int step_h = height / 2;

    for (std::string::size_type i = 0; i < str.size(); i++)
    {
        int sym = static_cast<int>(str.c_str()[i]);
        if (sym == 2)
            x += step_w;
        else if (sym == 3)
            y += step_h;
        else if (sym == 4)
        {
            x += step_w;
            y += step_h;
        }
        step_w /= 2;
        step_h /= 2;
    }
    image(cv::Range(y, y + h), cv::Range(x, x + w)).setTo(color);
}

void old_merge_image(cv::Mat image, double stddev)
{
    const auto width = image.cols;
    const auto height = image.rows;
    if (image.empty())
        return; 
    cv::Mat quarter1 = image(cv::Range(0, height / 2), cv::Range(0, width / 2));
    cv::Mat quarter2 = image(cv::Range(0, height / 2), cv::Range(width / 2, width));
    cv::Mat quarter4 = image(cv::Range(height / 2, height), cv::Range(width / 2, width));
    cv::Mat quarter3 = image(cv::Range(height / 2, height), cv::Range(0, width / 2));
    if (quarter1.empty() || quarter2.empty() || quarter3.empty() || quarter4.empty())
        return; 
    cv::Mat mean1, mean2, mean3, mean4;
    cv::Mat dev1, dev2, dev3, dev4;
    cv::meanStdDev(quarter1, mean1, dev1);
    cv::meanStdDev(quarter2, mean2, dev2);
    cv::meanStdDev(quarter3, mean3, dev3);
    cv::meanStdDev(quarter4, mean4, dev4);

    bool q1 = false;
    bool q2 = false;
    bool q3 = false;
    bool q4 = false;

    if (dev1.at<double>(0) <= stddev && dev2.at<double>(0) <= stddev)
    {
        if (abs(mean1.at<double>(0) - mean2.at<double>(0)) <= 10)
        {
            cv::Mat mean = (mean1 + mean2) / 2;
            quarter1.setTo(mean);
            quarter2.setTo(mean);
            q1 = true;
            q2 = true;
        }
    }

    if (dev4.at<double>(0) <= stddev && dev2.at<double>(0) <= stddev)
    {
        if (abs(mean4.at<double>(0) - mean2.at<double>(0)) <= 10)
        {
            cv::Mat mean = (mean4 + mean2) / 2;
            quarter4.setTo(mean);
            quarter2.setTo(mean);
            q4 = true;
            q2 = true;
        }
    }

    if (dev1.at<double>(0) <= stddev && dev3.at<double>(0) <= stddev)
    {
        if (abs(mean1.at<double>(0) - mean3.at<double>(0)) <= 10)
        {
            cv::Mat mean = (mean1 + mean3) / 2;
            quarter1.setTo(mean);
            quarter3.setTo(mean);
            q1 = true;
            q3 = true;
        }
    }

    if (dev4.at<double>(0) <= stddev && dev3.at<double>(0) <= stddev)
    {
        if (abs(mean4.at<double>(0) - mean3.at<double>(0)) <= 10)
        {
            cv::Mat mean = (mean4 + mean3) / 2;
            quarter4.setTo(mean);
            quarter3.setTo(mean);
            q3 = true;
            q4 = true;
        }
    }
    
    if (!q1)
        old_merge_image(quarter1, stddev);
    if (!q2)
        old_merge_image(quarter2, stddev);
    if (!q3)
        old_merge_image(quarter3, stddev); 
    if (!q4)
        old_merge_image(quarter4, stddev);
}

void merge_image(cv::Mat image, double stddev)
{
    for (int i = 0; i < gIndeces.size(); i++)
    {
        for (int j = i; j < gIndeces.size(); j++)
        {
            int idx_i = gIndeces.at(i);
            int idx_j = gIndeces.at(j);
            if (neighbour_indeces(idx_i, idx_j))
            {
                cv::Mat mean_i, mean_j, mean_ij;
                cv::Mat dev_i, dev_j, dev_ij;
                cv::Mat area_i, area_j, area_ij;

                area_by_index(idx_i, image, area_i);
                area_by_index(idx_j, image, area_j);

                cv::Mat a = area_i.clone();
                cv::Mat b;
                cv::hconcat(area_i.clone().reshape(0, 1), area_j.clone().reshape(0, 1), area_ij);

                cv::meanStdDev(area_i, mean_i, dev_i);
                cv::meanStdDev(area_j, mean_j, dev_j);
                cv::meanStdDev(area_ij, mean_ij, dev_ij);

                if (dev_i.at<double>(0) <= stddev && dev_j.at<double>(0) <= stddev && dev_ij.at<double>(0) <= stddev)
                {
                    if (abs(mean_i.at<double>(0) - mean_j.at<double>(0)) <= 10)
                    {
                        set_to_by_index(image, idx_i, mean_ij);
                        set_to_by_index(image, idx_j, mean_ij);
                    }
                }
            }
        }
    }
}
}

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev)
{
    cv::Mat res = image;
    split_image(res, stddev, 0);
    merge_image(res, stddev);
    return res;
}
}