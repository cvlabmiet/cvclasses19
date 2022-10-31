/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <cstring>
#include <cstdlib>

namespace
{
void area_by_index(int index, cv::Mat image, cv::Mat roi)
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

    roi = image(cv::Range(y, y + h), cv::Range(x, x + w));
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
                    dir = 1; //вправо
                if (j1 == 3 && j2 == 4)
                    dir = 1; //вправо
                if (j1 == 1 && j2 == 3)
                    dir = 2; //вниз
                if (j1 == 2 && j2 == 4)
                    dir = 2; //вниз
                if (j1 == 2 && j2 == 1)
                    dir = 3; //влево
                if (j1 == 4 && j2 == 3)
                    dir = 3; //влево
                if (j1 == 3 && j2 == 1)
                    dir = 4; //вверх
                if (j1 == 4 && j2 == 2)
                    dir = 4; //вверх
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

void split_image(cv::Mat image, double stddev)
{
    const auto width = image.cols;
    const auto height = image.rows;
    if (image.empty())
        return; 

    cv::Mat mean;
    cv::Mat dev;
    cv::meanStdDev(image, mean, dev);

    if (dev.at<double>(0) <= stddev)
    {
        image.setTo(mean);
        return;
    }

    split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev);
    split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev);
    split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev);
    split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev);
}


void merge_image(cv::Mat image, double stddev)
{
    const auto width = image.cols;
    const auto height = image.rows;
    if (image.empty())
        return; 
    cv::Mat quarter1 = image(cv::Range(0, height / 2), cv::Range(0, width / 2));
    cv::Mat quarter2 = image(cv::Range(0, height / 2), cv::Range(width / 2, width));
    cv::Mat quarter3 = image(cv::Range(height / 2, height), cv::Range(width / 2, width));
    cv::Mat quarter4 = image(cv::Range(height / 2, height), cv::Range(0, width / 2));
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

    //pair 1-2
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

    //pair 2-4
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

    //pair 1-3
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

    //pair 3-4
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
    
    if (!q1) merge_image(quarter1, stddev);
    if (!q2) merge_image(quarter2, stddev);
    if (!q3) merge_image(quarter3, stddev); 
    if (!q4) merge_image(quarter4, stddev);
}
} // namespace

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev)
{
    // split part
    cv::Mat res = image;
    split_image(res, stddev);

    // merge part
    merge_image(res, stddev);
    return res;
}
} // namespace cvlib
