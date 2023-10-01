/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2023-10-01
 * @author Rururustm
 */

#include "cvlib.hpp"
#include <vector>

struct area
{
    int x;
    int y;
    int size_x;
    int size_y;
};

namespace
{
void split_image(cv::Mat image, double stddev, std::vector<area> &regions, area cur_area)
{
    if (image.empty())
    return;
    cv::Mat mean;
    cv::Mat dev;
    cv::meanStdDev(image, mean, dev);

    if (dev.at<double>(0) <= stddev)
    {
        regions.push_back(cur_area);
        image.setTo(mean);
        return;
    }

    const auto width = image.cols;
    const auto height = image.rows;

    split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev, regions, {.x = 0 + cur_area.x, .y = 0 + cur_area.y, .size_x = height / 2, .size_y = width / 2});
    split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev, regions, {.x = 0 + cur_area.x, .y = width / 2 + cur_area.y, .size_x = height / 2, .size_y = width / 2});
    split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev, regions, {.x = height / 2 + cur_area.x, .y = width / 2 + cur_area.y, .size_x = height / 2, .size_y = width / 2});
    split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev, regions, {.x = height / 2 + cur_area.x, .y = 0 + cur_area.y, .size_x = height / 2, .size_y = width / 2});
}

void merge_image(cv::Mat &image, double stddev, std::vector<area> &regions)
{
	cv::Mat vec_i;
        cv::Mat vec_j;
        cv::Mat im_union;
        cv::Mat mean;
        cv::Mat dev;
    for(size_t i = 0; i < regions.size(); ++i)
    {
    size_t flag = 0;
        for(size_t j = i; (j < regions.size()) && (flag < (regions[i].size_x + regions[i].size_y + 1)); ++j)
        {
            if (((regions[i].x + regions[i].size_x == regions[j].x) || (regions[i].y + regions[i].size_y == regions[j].y))
            && (regions[i].x <= regions[j].x && regions[j].x <= regions[i].x + regions[i].size_x)
            && (regions[i].y <= regions[j].y && regions[j].y <= regions[i].y + regions[i].size_y)
            && regions[i].size_x != 0 && regions[i].size_y != 0 && regions[j].size_x != 0 && regions[j].size_y != 0 )
            {
                (image(cv::Range(regions[i].x, regions[i].x + regions[i].size_x), cv::Range(regions[i].y, regions[i].y + regions[i].size_y))).copyTo(vec_i);
                (image(cv::Range(regions[j].x, regions[j].x + regions[j].size_x), cv::Range(regions[j].y, regions[j].y + regions[j].size_y))).copyTo(vec_j);
                vec_i = vec_i.reshape(0, 1);
                vec_j = vec_j.reshape(0, 1);
                cv::hconcat(vec_i, vec_j, im_union);
                cv::meanStdDev(im_union, mean, dev);

                if (dev.at<double>(0) <= stddev)
                {
                    image(cv::Range(regions[i].x, regions[i].x + regions[i].size_x), cv::Range(regions[i].y, regions[i].y + regions[i].size_y)).setTo(mean);
                    image(cv::Range(regions[j].x, regions[j].x + regions[j].size_x), cv::Range(regions[j].y, regions[j].y + regions[j].size_y)).setTo(mean);
                    ++ flag;
                }
            }
        }
    }
}
} // namespace

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev)
{
    // split part
    std::vector<area> p_regions;
    cv::Mat res = image;
    area start_area = {.x = 0, .y = 0, .size_x = res.cols, .size_y = res.rows};
    split_image(res, stddev, p_regions, start_area);

    // merge part
    merge_image(res, stddev, p_regions);
    return res;
}
} // namespace cvlib
