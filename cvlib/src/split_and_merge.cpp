/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-17
 * @author Vladislav Dimakov
 */

#include "cvlib.hpp"
#include <vector>

namespace
{
struct Region
{
    bool isActive;
    std::vector<cv::Rect> areas;
    std::vector<unsigned int> neighbors;
    unsigned int pixelNum;
    unsigned int sum;
    unsigned int quadSum;
};

class SplitAndMerge
{
    public:
    void Process(const cv::Mat& image, cv::Mat splitImage, cv::Mat mergeImage, double stddev, int minSquare, double meanDeviation, double scaleFactor)
    {
        m_regions.clear();

        image.copyTo(m_image);
        m_splitImage = cv::Mat(image.rows, image.cols, image.type(), cv::Scalar(0));
        m_mergeImage = cv::Mat(image.rows, image.cols, image.type(), cv::Scalar(0));
        m_labels = cv::Mat(image.rows, image.cols, CV_32SC1, cv::Scalar(-1));

        if (!m_image.empty())
        {
            cv::Rect area(0, 0, image.cols, image.rows);
            splitProcess(area, stddev, minSquare);

            findNeighbors();
            mergeProcess(stddev, meanDeviation, scaleFactor);
            makeMergeImage();
        }

        m_splitImage.copyTo(splitImage);
        m_mergeImage.copyTo(mergeImage);
    }

    private:
    void splitProcess(cv::Rect area, double stddev, int minSquare)
    {
        unsigned int curPixelNum = area.width * area.height;
        unsigned int curSum = 0;
        unsigned int curQuadSum = 0;

        for (size_t i = area.y; i < area.y + area.height; i++)
        {
            for (size_t j = area.x; j < area.x + area.width; j++)
            {
                curSum += m_image.at<unsigned char>(i, j);
                curQuadSum += m_image.at<unsigned char>(i, j) * m_image.at<unsigned char>(i, j);
            }
        }

        double curMean = curSum / double(curPixelNum);
        double curQuadMean = curQuadSum / double(curPixelNum);
        double curStddev = sqrt(curQuadMean - curMean * curMean);

        if (curStddev <= stddev || area.width == 1 || area.height == 1 || area.width * area.height <= minSquare)
        {
            cv::rectangle(m_splitImage, area, cv::Scalar(curSum / double(curPixelNum) + 0.5), -1);
            cv::rectangle(m_labels, area, cv::Scalar(m_regions.size()), 1);

            Region region;
            region.isActive = true;
            region.areas.push_back(area);
            region.pixelNum = curPixelNum;
            region.sum = curSum;
            region.quadSum = curQuadSum;

            m_regions.push_back(region);

            return;
        }

        int x = area.x;
        int y = area.y;
        double halfWidth = area.width / 2.0;
        double halfHeight = area.height / 2.0;

        cv::Rect area1(x, y, halfWidth, halfHeight);
        splitProcess(area1, stddev, minSquare);

        cv::Rect area2(x + halfWidth, y, halfWidth + 0.5, halfHeight);
        splitProcess(area2, stddev, minSquare);

        cv::Rect area3(x, y + halfHeight, halfWidth, halfHeight + 0.5);
        splitProcess(area3, stddev, minSquare);

        cv::Rect area4(x + halfWidth, y + halfHeight, halfWidth + 0.5, halfHeight + 0.5);
        splitProcess(area4, stddev, minSquare);
    }

    void findNeighbors()
    {
        int curValue, leftValue, rightValue, botValue, topValue;

        for (size_t i = 0; i < m_labels.rows; i++)
        {
            for (size_t j = 0; j < m_labels.cols; j++)
            {
                curValue = m_labels.at<int>(i, j);
                if (curValue == -1)
                    continue;

                if (j > 0)
                {
                    leftValue = m_labels.at<int>(i, j - 1);

                    if (leftValue != -1 && curValue != leftValue)
                    {
                        if (std::find(m_regions[curValue].neighbors.begin(), m_regions[curValue].neighbors.end(), leftValue) ==
                            m_regions[curValue].neighbors.end())
                        {
                            m_regions[curValue].neighbors.push_back(leftValue);
                        }
                    }
                }

                if (j + 1 < m_labels.cols)
                {
                    rightValue = m_labels.at<int>(i, j + 1);

                    if (rightValue != -1 && curValue != rightValue)
                    {
                        if (std::find(m_regions[curValue].neighbors.begin(), m_regions[curValue].neighbors.end(), rightValue) ==
                            m_regions[curValue].neighbors.end())
                        {
                            m_regions[curValue].neighbors.push_back(rightValue);
                        }
                    }
                }

                if (i > 0)
                {
                    botValue = m_labels.at<int>(i - 1, j);

                    if (botValue != -1 && curValue != botValue)
                    {
                        if (std::find(m_regions[curValue].neighbors.begin(), m_regions[curValue].neighbors.end(), botValue) ==
                            m_regions[curValue].neighbors.end())
                        {
                            m_regions[curValue].neighbors.push_back(botValue);
                        }
                    }
                }

                if (i + i < m_labels.rows)
                {
                    topValue = m_labels.at<int>(i + 1, j);

                    if (topValue != -1 && curValue != topValue)
                    {
                        if (std::find(m_regions[curValue].neighbors.begin(), m_regions[curValue].neighbors.end(), topValue) ==
                            m_regions[curValue].neighbors.end())
                        {
                            m_regions[curValue].neighbors.push_back(topValue);
                        }
                    }
                }
            }
        }
    }

    bool mergeRegionWithNeighbors(size_t regionLabel, double stddev, double meanDeviation, double scaleFactor)
    {
        for (std::vector<unsigned int>::iterator neighbor = m_regions[regionLabel].neighbors.begin();
             neighbor != m_regions[regionLabel].neighbors.end(); neighbor++)
        {
            if (!m_regions[*neighbor].isActive)
                continue;

            unsigned int mergedSum = m_regions[regionLabel].sum + m_regions[*neighbor].sum;
            unsigned int mergedQuadSum = m_regions[regionLabel].quadSum + m_regions[*neighbor].quadSum;
            unsigned int mergedPixelNum = m_regions[regionLabel].pixelNum + m_regions[*neighbor].pixelNum;

            double mean = m_regions[regionLabel].sum / double(m_regions[regionLabel].pixelNum);
            double mergedMean = mergedSum / double(mergedPixelNum);
            double mergedQuadMean = mergedQuadSum / double(mergedPixelNum);
            double mergedStddev = sqrt(mergedQuadMean - mergedMean * mergedMean);

            if (abs(mergedMean - mean) <= meanDeviation && m_regions[regionLabel].pixelNum / double(m_regions[*neighbor].pixelNum) <= scaleFactor &&
                m_regions[regionLabel].pixelNum / double(m_regions[*neighbor].pixelNum) >= 1 / scaleFactor)
            {
                m_regions[*neighbor].isActive = false;

                m_regions[regionLabel].sum = mergedSum;
                m_regions[regionLabel].quadSum = mergedQuadSum;
                m_regions[regionLabel].pixelNum = mergedPixelNum;

                for (auto& newArea : m_regions[*neighbor].areas)
                    m_regions[regionLabel].areas.push_back(newArea);

                for (unsigned int newNeighbor : m_regions[*neighbor].neighbors)
                {
                    if (newNeighbor != regionLabel)
                    {
                        m_regions[regionLabel].neighbors.push_back(newNeighbor);
                    }
                }

                return true;
            }
        }

        return false;
    }

    void mergeProcess(double stddev, double meanDeviation, double scaleFactor)
    {
        bool needToStop = false;
        while (!needToStop)
        {
            needToStop = true;
            for (size_t regionNum = 0; regionNum < m_regions.size(); regionNum++)
            {
                if (m_regions[regionNum].isActive)
                {
                    if (mergeRegionWithNeighbors(regionNum, stddev, meanDeviation, scaleFactor))
                    {
                        needToStop = false;
                    }
                }
            }
        }
    }

    void makeMergeImage()
    {
        for (auto& region : m_regions)
        {
            if (region.isActive)
            {
                unsigned char mean = region.sum / double(region.pixelNum) + 0.5;
                for (auto& area : region.areas)
                {
                    cv::rectangle(m_mergeImage, area, cv::Scalar(mean), -1);
                }
            }
        }
    }

    std::vector<Region> m_regions;
    cv::Mat m_image;
    cv::Mat m_splitImage;
    cv::Mat m_mergeImage;
    cv::Mat m_labels;
};
} // namespace

namespace cvlib
{
void split_and_merge(const cv::Mat& image, cv::Mat splitImage, cv::Mat mergeImage, double stddev, int minSquare, double meanDeviation, double scaleFactor)
{
    SplitAndMerge splitAndMerge;
    splitAndMerge.Process(image, splitImage, mergeImage, stddev, minSquare, meanDeviation, scaleFactor);
}
} // namespace cvlib
