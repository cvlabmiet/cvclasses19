/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2022-09-25
 * @author Suetolog
 */

#include "cvlib.hpp"
#include <cmath>

namespace 
{
struct SquareRegion
{
    cv::Rect roi;
    cv::Mat mean;
    cv::Mat std;

    SquareRegion(int x0, int y0, int height, int width, cv::Mat mean, cv::Mat std)
    {
        roi.x = x0;
        roi.y = y0;
        roi.height = height;
        roi.width = width;
        this->mean = mean;
        this->std = std;
    }

    bool intersects(SquareRegion& sreg)
    {
        bool res = false;
        int rx0 = sreg.roi.x;
        int ry0 = sreg.roi.y;
        int rx1 = rx0 + sreg.roi.width;
        int ry1 = ry0 + sreg.roi.height;

        res = (belongs(roi.x, roi.y, roi.x + roi.width, roi.y, rx1, ry1) && belongs(roi.x, roi.y, roi.x + roi.width, roi.y, rx0, ry1));

        res = res || (belongs(roi.x + roi.width, roi.y, roi.x + roi.width, roi.y + roi.height, rx0, ry0) &&
                      belongs(roi.x + roi.width, roi.y, roi.x + roi.width, roi.y + roi.height, rx0, ry1));

        res = res || (belongs(roi.x + roi.width, roi.y + roi.height, roi.x, roi.y + roi.height, rx0, ry0) &&
                      belongs(roi.x + roi.width, roi.y + roi.height, roi.x, roi.y + roi.height, rx1, ry0));

        res = res || (belongs(roi.x, roi.y, roi.x, roi.y + roi.height, rx1, ry0) && belongs(roi.x, roi.y, roi.x, roi.y + roi.height, rx1, ry1));

        return res;
    }
    int dist(int x0, int y0, int x1, int y1)
    {
        return (x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1);
    }
    bool belongs(int x0, int y0, int x1, int y1, int x_check, int y_check)
    {
        int dist1 = dist(x0, y0, x1, y1);
        int dist2 = dist(x0, y0, x_check, y_check) + dist(x_check, y_check, x1, y1);
        return dist1 == dist2;
    }
    // cv::Mat pixels;
};

struct Region
{
    std::vector<SquareRegion> sregions;
    //cv::Mat mask;

    bool validity = true;
    double RegionStd;
    double RegionStd2;

    Region()
    {
        RegionStd = 0;
        RegionStd2 = 0;
        
    }

    Region(SquareRegion sreg)
    {
        sregions.emplace_back(sreg);
        RegionStd = sreg.std.at<double>(0);
        RegionStd2 = RegionStd * RegionStd;
    }

    void addSquareReg(SquareRegion sreg)
    {
        sregions.emplace_back(sreg);
        double tmp = sreg.std.at<double>(0);
        RegionStd2 = RegionStd2 + pow(tmp, 2);
        RegionStd = sqrt(RegionStd2);
    }

    void addSquareReg(SquareRegion sreg, double newStd, double newStd2)
    {
        sregions.emplace_back(sreg);
        RegionStd2 = newStd2;
        RegionStd = newStd;
    }

    double getRegMean()
    {
        double res;
        res = sregions[0].mean.at<double>(0);
        if (sregions.size() > 1)
        {
            for (auto i = 1; i < sregions.size(); i++)
            {
                res += sregions[i].mean.at<double>(0);
            }
            res = res / sregions.size();
        }
        return (res);
    }
};

cv::Mat create_mask(Region reg, int img_cols, int img_rows)
{
    cv::Mat mask = cv::Mat::zeros(cv::Size(img_cols, img_rows), CV_8U);

    for (auto& i : reg.sregions)
    {
        mask(cv::Rect(i.roi.x, i.roi.y, i.roi.width, i.roi.height)) = 255;
    }
    return mask;
}

bool have_common_sregions(Region reg1, Region reg2)
{
    for (auto& i : reg1.sregions)
    {
        for (auto& j : reg2.sregions)
        {
            if (i.intersects(j) || j.intersects(i))
                return true;
        }
    }
    return false;
}

void split_image(int x0, int y0, cv::Mat& image, std::vector<SquareRegion>& regs, double stddev, int minWidth, int minHeight)
{
    cv::Mat mean;
    cv::Mat dev;

    const auto width = image.cols;
    const auto height = image.rows;

    cv::meanStdDev(image, mean, dev);
    double debug_mean = mean.at<double>(0);
    double debug_dev = dev.at<double>(0);

    if ((dev.at<double>(0) <= stddev) || (width < minWidth) || (height < minHeight))
    {
        regs.emplace_back(SquareRegion(x0, y0, height, width, mean, dev));
        return;
    }

    split_image(x0, y0, image(cv::Range(0, height / 2), cv::Range(0, width / 2)), regs, stddev, minWidth, minHeight);
    split_image(x0 + width / 2, y0, image(cv::Range(0, height / 2), cv::Range(width / 2, width)), regs, stddev, minWidth, minHeight);
    split_image(x0 + width / 2, y0 + height / 2, image(cv::Range(height / 2, height), cv::Range(width / 2, width)), regs, stddev, minWidth, minHeight);
    split_image(x0, y0 + height / 2, image(cv::Range(height / 2, height), cv::Range(0, width / 2)), regs, stddev, minWidth, minHeight);
}
}


namespace cvlib
{
    cv::Mat split_and_merge(const cv::Mat& image, double stddev, int minWidth, int minHeight)
    {
        // split part
        cv::Mat res = image;
        std::vector<SquareRegion> sregs;

        split_image(0, 0, res, sregs, stddev, minWidth, minHeight);

        std::cout << "Regions count: " << sregs.size() << std::endl;

        std::vector<Region> regions(sregs.size());
        for (auto i = 0; i < sregs.size(); i++)
        {
            regions[i] = Region(sregs[i]);
        }

        std::vector<bool> used(sregs.size());
        for (int i = 0; i < regions.size(); i++)
        {
            for (int j = i + 1; j < regions.size(); j++)
            {
                if (used[i] || used[j])
                    continue;
                ;
                
                bool predicate = false;
                if ((regions[i].RegionStd2 == 0) || (regions[j].RegionStd2 == 0))
                {
                    if (abs(regions[i].getRegMean() - regions[j].getRegMean()) <= stddev)
                        predicate = true;
                }
                else if (sqrt(regions[i].RegionStd2 + regions[j].RegionStd2) < stddev)
                    predicate = true;

                if (predicate) //check predicate (Ri v Rj)
                {
                    if (have_common_sregions(regions[i], regions[j]))
                    {
                        used[j] = true;
                        for (int k = 0; k < regions[j].sregions.size(); k++)
                        {
                            regions[i].addSquareReg(regions[j].sregions[k]);
                        }
                        regions[j].validity = false;
                    }
                }
            }
        }
        for (int i = 0; i < regions.size(); i++)
        {
            if (regions[i].validity == false)
            {
                continue;
            }
            cv::Mat mask = create_mask(regions[i], res.cols, res.rows);
            cv::Mat mean;
            cv::Mat dev;
            cv::meanStdDev(res, mean, dev, mask);
            res.setTo(mean, mask);
        }
        return res;
    }

} // namespace cvlib
