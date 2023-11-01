/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"

namespace
{

struct four_Zones
{
    cv::Mat image;
    std::vector<four_Zones> inner_Zones;
    bool inner_split_flag = false;

    four_Zones(cv::Mat _image)
    {
        image = _image;
    }
};

void split_image(cv::Mat image, double stddev, four_Zones* curr_Zones)
{
    cv::Mat mean;
    cv::Mat dev;
    cv::meanStdDev(image, mean, dev);

    if (dev.at<double>(0) <= stddev)
    {
        image.setTo(mean);
        return;
    }

    const auto width = image.cols;
    const auto height = image.rows;

    curr_Zones->image = image;

    if (width < 2 || height < 2)
        return;

    curr_Zones->inner_split_flag = true;

    curr_Zones->inner_Zones.push_back(four_Zones(image(cv::Range(0, height / 2), cv::Range(0, width / 2))));
    curr_Zones->inner_Zones.push_back(four_Zones(image(cv::Range(0, height / 2), cv::Range(width / 2, width))));
    curr_Zones->inner_Zones.push_back(four_Zones(image(cv::Range(height / 2, height), cv::Range(0, width / 2))));
    curr_Zones->inner_Zones.push_back(four_Zones(image(cv::Range(height / 2, height), cv::Range(width / 2, width))));

    split_image(curr_Zones->inner_Zones[0].image, stddev, &curr_Zones->inner_Zones[0]);
    split_image(curr_Zones->inner_Zones[1].image, stddev, &curr_Zones->inner_Zones[1]);
    split_image(curr_Zones->inner_Zones[2].image, stddev, &curr_Zones->inner_Zones[2]);
    split_image(curr_Zones->inner_Zones[3].image, stddev, &curr_Zones->inner_Zones[3]);
};

void merge_two_zones(four_Zones* imZones1, four_Zones* imZones2, double stddev)
{

    cv::Mat mean1, mean2, dev1, dev2;

    cv::meanStdDev(imZones1->image, mean1, dev1);
    cv::meanStdDev(imZones2->image, mean2, dev2);

    if ((dev1.at<double>(0) <= stddev) && (dev2.at<double>(0) <= stddev))
    {
        imZones1->image.setTo((mean1 + mean2) / 2);
        imZones2->image.setTo((mean1 + mean2) / 2);
    }
};

void merge_image(double stddev, four_Zones* curr_Zones)
{
    while (curr_Zones->inner_split_flag)
    {
        if (curr_Zones->inner_Zones[0].inner_split_flag ||\
            curr_Zones->inner_Zones[1].inner_split_flag ||\
            curr_Zones->inner_Zones[2].inner_split_flag ||\
            curr_Zones->inner_Zones[3].inner_split_flag)
        {
            merge_image(stddev, &curr_Zones->inner_Zones[0]);
            merge_image(stddev, &curr_Zones->inner_Zones[1]);
            merge_image(stddev, &curr_Zones->inner_Zones[2]);
            merge_image(stddev, &curr_Zones->inner_Zones[3]);
        }
        else
        {
            curr_Zones->inner_split_flag = false;
            merge_two_zones(&curr_Zones->inner_Zones[0], &curr_Zones->inner_Zones[1], stddev);
            merge_two_zones(&curr_Zones->inner_Zones[0], &curr_Zones->inner_Zones[2], stddev);
            merge_two_zones(&curr_Zones->inner_Zones[1], &curr_Zones->inner_Zones[3], stddev);
            merge_two_zones(&curr_Zones->inner_Zones[2], &curr_Zones->inner_Zones[3], stddev);
        }
    }
};

} // namespace


namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev)
{
// split part
    cv::Mat res = image;

    four_Zones imZones(image);

    split_image(res, stddev, &imZones);

    merge_image(stddev, &imZones);
    return res;
}
} // namespace cvlib
