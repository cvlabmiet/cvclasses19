/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <vector>
#include <list>

namespace SaM
{
    struct RoI
    {
        std::vector<cv::Rect> region_parts;
        int x, y, w, h;
        int accum_mean = 0;
        int accum_std = 0;

        RoI(){}
        RoI(int x, int y, int w, int h):x(x), y(y), w(w), h(h)
        {
            region_parts.emplace_back(x,y, w,h);
        }

        void appendRegion(const RoI& roi)
        {
            region_parts.insert(region_parts.end(), roi.region_parts.begin(), roi.region_parts.end());
            accum_mean += roi.accum_mean;
            accum_std += roi.accum_std;
        }

        void set(int x, int y, int w, int h)
        {
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
            region_parts.clear();
            region_parts.emplace_back(x,y, w,h);
        }

        int getMean()
        {
            return accum_mean / region_parts.size();
        }

        int getStddev()
        {
            return accum_std / std::sqrt(region_parts.size());
        }

        cv::Rect toRect()
        {
            return cv::Rect(x, y, w, h);
        }

        bool isNeighbour( const RoI& roi)
        {
            for(const auto& rect1: region_parts)
            {
                for(const auto& rect2: roi.region_parts)
                {
                    int dx = rect2.x - rect1.x;
                    int dy = rect2.y - rect1.y;
                    
                    if(dx == rect1.width ) // может оказаться соседом справа
                    {
                        if(rect1.y >= rect2.y && rect1.y <= rect2.y + rect2.height)
                            return true;
                        if(rect2.y >= rect1.y && rect2.y <= rect1.y + rect1.height)
                            return true;
                        else
                            continue;
                    } 
                    
                    if(dy == rect1.height) // может оказаться соседом снизу
                    {
                        if(rect1.x >= rect2.x && rect1.x <= rect2.x + rect2.width)
                            return true;
                        if(rect2.x >= rect1.x && rect2.x <= rect1.x + rect1.width)
                            return true;
                        else
                            continue;
                    }

                    if(dx == -rect2.width) // может оказаться соседом слева
                    {
                        if(rect1.y >= rect2.y && rect1.y <= rect2.y + rect2.height)
                            return true;
                        if(rect2.y >= rect1.y && rect2.y <= rect1.y + rect1.height)
                            return true;
                        else
                            continue;
                    }

                    if(dy == -rect2.height) // может оказаться соседом сверху
                    {
                        if(rect1.x >= rect2.x && rect1.x <= rect2.x + rect2.width)
                            return true;
                        if(rect2.x >= rect1.x && rect2.x <= rect1.x + rect1.width)
                            return true;
                        else
                            continue;
                    }
                }
            }
            return false;
        } 
    };

    int g_mean;
    int g_stddev;
    cv::Mat g_image;
    std::vector<RoI> rois;
    std::vector<bool> merged;

    void split_image(RoI& roi)
    {
        cv::Mat mean;
        cv::Mat dev;
        cv::meanStdDev(g_image( roi.toRect()), mean, dev);

        if (dev.at<double>(0) <= g_stddev || roi.w < 5 || roi.h < 5)
        {
            rois.emplace_back(roi.x, roi.y, roi.w, roi.h );
            rois.back().accum_mean = mean.at<double>(0);
            rois.back().accum_std = dev.at<double>(0);          
            return;
        }

        int x, y, half_w, half_h;
        x = roi.x;
        y = roi.y;
        half_h = roi.h/2;
        half_w = roi.w/2;

        roi.set(x, y, half_w, half_h);
        split_image(roi);
        roi.set(x + half_w, y, half_w, half_h);
        split_image(roi);
        roi.set(x + half_w, y + half_h, half_w, half_h);
        split_image(roi);
        roi.set(x, y + half_h, half_w, half_h);
        split_image(roi);
    } 

    void merge_rois()
    {
        merged.clear();
        merged.resize(rois.size(), false);

        bool is_merge = true;
        while (is_merge){
            is_merge = false;
            for(int i = 0; i < rois.size() - 1; i++)
                if(!merged[i])
                    for(int j = i+1; j < rois.size(); j++)
                        if(!merged[j])
                            if(rois[i].isNeighbour(rois[j]))
                                if(std::abs(rois[i].getMean() - rois[j].getMean()) < g_mean &&
                                   std::abs(rois[i].getStddev() - rois[j].getStddev()) < g_stddev)
                                {
                                    merged[j] = true;
                                    rois[i].appendRegion(rois[j]);
                                    is_merge = true;
                                }                                   
        }
    }
} // nspace SaM

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev, int mean)
{
    SaM::rois.clear();
    SaM::g_image = image;
    SaM::g_stddev = stddev;
    SaM::g_mean = mean;

// split part
    SaM::RoI roi(0, 0, image.cols, image.rows);
    SaM::split_image(roi);
        
// merge part
    SaM::merge_rois();
    for(int i = 0; i < SaM::merged.size(); i++)
        if(!SaM::merged[i])
            for(const auto& rect: SaM::rois[i].region_parts)
            {
                SaM::g_image(rect).setTo(SaM::rois[i].getMean());
            }
    return SaM::g_image;
}

} // namespace cvlib