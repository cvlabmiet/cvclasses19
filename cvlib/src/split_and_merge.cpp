/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"
class region
{
    public:
    int x, y, h, w;
    double min_val = -1, max_val = -1;
    double mean = -1;
    std::vector<std::pair<int, int>> points;
    region(int x, int y, int h, int w)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        points.emplace_back(std::make_pair(x, y));
        points.emplace_back(std::make_pair(x + w, y));
        points.emplace_back(std::make_pair(x + w, y + h));
        points.emplace_back(std::make_pair(x, y + h));
    }
    bool intersection(region& r1)
    {
        bool res = false;
        res = (belongs(this->points[0], this->points[1], r1.points[2]) && belongs(this->points[0], this->points[1], r1.points[3]));

        res = res || (belongs(this->points[1], this->points[2], r1.points[0]) && belongs(this->points[1], this->points[2], r1.points[3]));

        res = res || (belongs(this->points[2], this->points[3], r1.points[0]) && belongs(this->points[2], this->points[3], r1.points[1]));

        res = res || (belongs(this->points[0], this->points[3], r1.points[1]) && belongs(this->points[0], this->points[3], r1.points[2]));
        //        for (int i = 0; i < 4; i++)
        //        {
        //            for (int j = 0; j < 4; j++)
        //            {
        //                res = res || (belongs(this->points[j], this->points[(j + 1) % 4], r1.points[i]) &&
        //                              belongs(this->points[j], this->points[(j + 1) % 4], r1.points[(i + 1) % 4]));
        //            }
        //        }
        //        for(auto &p:r1.points)
        //        {
        //            res = res || belongs(this->points[0],this->points[1],p);
        //            res += belongs(this->points[1],this->points[2],p);
        //            res += belongs(this->points[2],this->points[3],p);
        //            res += belongs(this->points[3],this->points[0],p);
        //        }
        return res;
    }
    int distance(std::pair<int, int> A, std::pair<int, int> B)
    {
        return (A.first - B.first) * (A.first - B.first) + (A.second - B.second) * (A.second - B.second);
    }
    bool belongs(std::pair<int, int> start, std::pair<int, int> end, std::pair<int, int> suspect)
    {
        int dist1 = distance(start, end);
        int dist2 = distance(start, suspect) + distance(suspect, end);
        return dist1 == dist2;
    }
};

std::vector<region> regions;
namespace
{
// void split_image(cv::Mat image, double stddev)
//{
//    cv::Mat mean;
//    cv::Mat dev;
//    cv::meanStdDev(image, mean, dev);
//
//    const auto width = image.cols;
//    const auto height = image.rows;
//
//    if (dev.at<double>(0) <= stddev || width <= 5 || height <= 5)
//    {
//        image.setTo(mean);
//        regions.back().mean = (mean.at<double>(0));
//        minMaxIdx(image, &regions.back().min_val, &regions.back().max_val);
//        return;
//    }
//
//    auto r = regions.back();
//    regions.pop_back();
//    regions.emplace_back(region(r.x, r.y, height / 2, width / 2));
//    split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev);
//    regions.emplace_back(region(r.x + width / 2, r.y, height / 2, width / 2));
//    split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev);
//    regions.emplace_back(region(r.x + width / 2, r.y + height / 2, height / 2, width / 2));
//    split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev);
//    regions.emplace_back(region(r.x, r.y + height / 2, height / 2, width / 2));
//    split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev);
//}

void split_image(cv::Mat image, double T)
{
    minMaxIdx(image, &regions.back().min_val, &regions.back().max_val);

    const auto width = image.cols;
    const auto height = image.rows;

    if ((regions.back().max_val - regions.back().min_val) <= T || width <= 5 || height <= 5)
    {
//        cv::Mat mean;
//        cv::Mat dev;
//        cv::meanStdDev(image, mean, dev);
//        image.setTo(mean);
//        regions.back().mean = (mean.at<double>(0));
        return;
    }

    auto r = regions.back();
    regions.pop_back();
    regions.emplace_back(region(r.x, r.y, height / 2, width / 2));
    split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), T);
    regions.emplace_back(region(r.x + width / 2, r.y, height / 2, width / 2));
    split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), T);
    regions.emplace_back(region(r.x + width / 2, r.y + height / 2, height / 2, width / 2));
    split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), T);
    regions.emplace_back(region(r.x, r.y + height / 2, height / 2, width / 2));
    split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), T);
}
} // namespace

bool have_common_region(std::vector<region>& field1, std::vector<region>& field2)
{
    for (auto& i : field1)
    {
        for (auto& j : field2)
        {
            if (i.intersection(j) || j.intersection(i))
                return true;
        }
    }
    return false;
}
cv::Mat create_mask(std::vector<region>& field1)
{
    cv::Mat mask = cv::Mat::zeros(cv::Size(480, 320), CV_8U);
    for (auto& i : field1)
    {
        mask(cv::Rect(i.x, i.y, i.w, i.h)) = 255;
    }
    return mask;
}

cv::Mat create_mask(std::vector<region>& field1, std::vector<region>& field2)
{
    cv::Mat mask = cv::Mat::zeros(cv::Size(320, 240), CV_8U);
    for (auto& i : field1)
    {
        mask(cv::Rect(i.x, i.y, i.w, i.h)) = 255;
    }
    for (auto& i : field2)
    {
        mask(cv::Rect(i.x, i.y, i.w, i.h)) = 255;
    }
    return mask;
}

namespace cvlib
{
// cv::Mat split_and_merge(const cv::Mat& image, double stddev)
//{
//    // split part
//    cv::Mat res = image;
//    regions.clear();
//    regions.emplace_back(region(0, 0, image.rows, image.cols));
//    split_image(res, stddev);
//    std::cout << regions.size() << std::endl;
//
//    std::vector<cv::Mat> masks;
//    std::vector<std::vector<region>> merges_regions(regions.size());
//    for (auto i = 0; i < regions.size(); i++)
//    {
//        merges_regions[i].push_back(regions[i]);
//        masks.push_back(create_mask(merges_regions[i]));
//    }
//
//    for (int i = 0; i < merges_regions.size(); i++)
//    {
//        for (int j = i + 1; j < merges_regions.size(); j++)
//        {
//            if (have_common_region(merges_regions[i], merges_regions[j]))
//            {
//                // cv::Mat mask = create_mask(merges_regions[i],merges_regions[j]);
//                cv::Mat mask = masks[i] + masks[j];
//                cv::Mat mean;
//                cv::Mat dev;
//                //                    cv::imshow("Mask", mask);
//                //                    cv::waitKey(400);
//                cv::meanStdDev(res, mean, dev, mask);
//                double std = dev.at<double>(0);
//                if (std <= stddev)
//                {
//                    merges_regions[i].insert(merges_regions[i].begin(), merges_regions[j].begin(), merges_regions[j].end());
//                    masks[i] = masks[i] + masks[j];
//                    merges_regions.erase(merges_regions.begin() + j);
//                    masks.erase(masks.begin() + j);
//                }
//            }
//        }
//    }
//    for (int i = 0; i < merges_regions.size(); i++)
//    {
//        // cv::Mat mask = create_mask(merges_regions[i]);
//        cv::Mat mask = masks[i];
//        cv::Mat mean;
//        cv::Mat dev;
//        cv::meanStdDev(res, mean, dev, mask);
//        res.setTo(mean, mask);
//    }
//    return res;
//}
cv::Mat split_and_merge(const cv::Mat& image, double T)
{
    // split part
    cv::Mat res = image;
    regions.clear();
    regions.emplace_back(region(0, 0, image.rows, image.cols));

    split_image(res, T);
    std::cout << "Regions count: " << regions.size() << std::endl;
    std::vector<cv::Mat> masks;
    std::vector<std::vector<region>> merges_regions(regions.size());
    for (auto i = 0; i < regions.size(); i++)
    {
        merges_regions[i].push_back(regions[i]);
        masks.push_back(create_mask(merges_regions[i]));
    }
    std::vector<bool> used(regions.size());
    for (int i = 0; i < merges_regions.size(); i++)
    {
        for (int j = i + 1; j < merges_regions.size(); j++)
        {
            if(used[i] || used[j]) continue;;
            double max_val = std::max(merges_regions[i][0].max_val, merges_regions[j][0].max_val);
            double min_val = std::min(merges_regions[i][0].min_val, merges_regions[j][0].min_val);
            if ((max_val - min_val) < T)
            {
                if (have_common_region(merges_regions[i], merges_regions[j]))
                {
                    used[j] = true;
                    merges_regions[i].insert(merges_regions[i].begin(), merges_regions[j].begin(), merges_regions[j].end());
                    masks[i] = masks[i] + masks[j];
                    merges_regions[i][0].min_val = min_val;
                    merges_regions[i][0].max_val = max_val;
                }
            }
        }
    }
    for (int i = 0; i < merges_regions.size(); i++)
    {
        cv::Mat mask = masks[i];
        cv::Mat mean;
        cv::Mat dev;
        cv::meanStdDev(res, mean, dev, mask);
        res.setTo(mean, mask);
    }
    return res;
}
} // namespace cvlib
