/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <vector>
#include <list>

namespace
{
	struct Region
	{
		unsigned int label;
		bool isActive;
		std::vector<cv::Rect> areas;
		std::list<unsigned int> neighbors;
		unsigned int pixelNum;
		unsigned int sum = 0;
		unsigned int sumQuad = 0;
		double stddev = 0.0;
	};

	class SplitAndMerge
	{
	public:
		SplitAndMerge()
		{
			
		}

		void Process(cv::Mat image, cv::Mat& splitImage, cv::Mat& mergeImage, double stddev)
		{
			m_regions.clear();
			labelNum = 0;

			m_labels = cv::Mat(image.rows, image.cols, CV_16U, cv::Scalar(0));
			
			image.copyTo(splitImage); // DELETE
			image.copyTo(m_image);
			image.copyTo(m_splitImage);
			
			cv::Rect area;
			area.x = 0; 
			area.y = 0;
			area.width = image.cols;
			area.height = image.rows;

			split_image(splitImage, area, stddev);
			m_splitImage.copyTo(splitImage);

			//image.copyTo(mergeImage);
			//make_labels_image();
			//find_neighbors();
			//merge_process(stddev);
			//make_merge_image(mergeImage);
		}
	private:
		double cals_stddev(unsigned int sum, unsigned int sumQuad, unsigned int pixelNum)
		{
			double M = sum / double(pixelNum);
			double MQuad = sumQuad / double(pixelNum);
			return sqrt(MQuad - M * M);
		}

		void split_image(cv::Mat &image, cv::Rect area, double stddev)
		{
			unsigned int pixelNum = image.rows * image.cols;
			unsigned int sum = 0;
			unsigned int sumQuad = 0;

			for (size_t i = area.y; i < area.y + area.height; i++)
			{
				for (size_t j = area.x; j < area.x + area.width; j++)
				{
					sum += m_image.at<unsigned char>(i, j);
					sumQuad += m_image.at<unsigned char>(i, j) * m_image.at<unsigned char>(i, j);
				}
			}

			double curStddev = cals_stddev(sum, sumQuad, pixelNum);

			if (curStddev <= stddev || image.rows == 1 || image.cols == 1)
			{
				cv::rectangle(m_splitImage, area, cv::Scalar(sum / pixelNum), -1);

				//Region region;
				//region.label = ++labelNum;
				//region.isActive = true;
				// TODO
				//m_regions.push_back(region);
				
				return;
			}

			area.width /= 2;
			area.height /= 2;
			const auto width = image.cols;
			const auto height = image.rows;

			cv::Rect area1 = area;
			split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), area1, stddev);

			cv::Rect area2 = area;
			area2.x += area.width;
			split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), area2, stddev);

			cv::Rect area3 = area;
			area3.y += area.height;
			area3.x += area.width;
			split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), area3, stddev);

			cv::Rect area4 = area;
			area4.y += area.height;
			split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), area4, stddev);
		}

		void make_labels_image()
		{
			for (auto &region : m_regions)
			{
				cv::rectangle(m_labels, region.areas[0], cv::Scalar(region.label));
			}
		}

		void find_neighbors()
		{
			for (auto &region : m_regions)
			{
				auto area = region.areas[0];
				auto neighbors = region.neighbors;

				if (area.x - 1>= 0)
				{
					neighbors.push_back(m_labels.at<unsigned short>(area.x - 1, area.y));
				}
				if (area.y - 1 >= 0)
				{
					neighbors.push_back(m_labels.at<unsigned short>(area.x, area.y - 1));
				}
				if (area.x + area.width + 1 < m_labels.cols)
				{
					neighbors.push_back(m_labels.at<unsigned short>(area.x + area.width + 1, area.y));
				}
				if (area.y + area.height + 1 < m_labels.rows)
				{
					neighbors.push_back(m_labels.at<unsigned short>(area.x, area.y + area.height + 1));
				}
			}
		}

		bool merge_regions_with_neighbors(Region &region, double stddev)
		{
			bool isMerged = false;
			for (auto &neighborLabel : region.neighbors)
			{
				if (!m_regions[neighborLabel].isActive || neighborLabel == region.label)
					continue;

				unsigned int sum = region.sum + m_regions[neighborLabel].sum;
				unsigned int sumQuad = region.sumQuad + m_regions[neighborLabel].sumQuad;
				unsigned int pixelNum = region.pixelNum + m_regions[neighborLabel].pixelNum;
				double mergedStddev = cals_stddev(sum, sumQuad, pixelNum);

				if (mergedStddev > stddev)
				{
					m_regions[neighborLabel].isActive = false;

					// Удалить m_regions[neighborLabel].label из region.neighbors

					for (auto &newNeighbor : m_regions[neighborLabel].neighbors)
					{
						if (newNeighbor != region.label)
							region.neighbors.push_back(newNeighbor);
					}		

					for (auto &newArea : m_regions[neighborLabel].areas)
						region.areas.push_back(newArea);

					region.sum = sum;
					region.sumQuad = sumQuad;
					region.pixelNum = pixelNum;

					isMerged = true;
				}
			}

			return isMerged;
		}

		void merge_process(double stddev)
		{
			bool needToStop = false;
			while (!needToStop)
			{
				needToStop = true;
				for (auto &region : m_regions)
				{
					if (region.isActive)
					{
						if (merge_regions_with_neighbors(region, stddev))
						{
							needToStop = false;
							continue;
						}
					}
				}
			}
		}

		void make_merge_image(cv::Mat &mergeImage)
		{
			for (auto &region : m_regions)
			{
				unsigned char mean = region.sum / region.pixelNum;
				for (auto &area : region.areas)
				{
					cv::rectangle(mergeImage, area, cv::Scalar(mean));
				}
			}
		}

		std::vector<Region> m_regions;
		cv::Mat m_image;
		cv::Mat m_splitImage;
		cv::Mat m_labels;
		unsigned int labelNum = 0;
	};
}

namespace cvlib
{
	cv::Mat split_and_merge(const cv::Mat& image, double stddev)
	{
		cv::Mat splitImage;
		cv::Mat mergeImage;
		SplitAndMerge splitAndMerge;

		splitAndMerge.Process(image, splitImage, mergeImage, stddev);
    
		return splitImage;
	}
} // namespace cvlib
