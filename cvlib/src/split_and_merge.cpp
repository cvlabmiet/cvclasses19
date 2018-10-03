/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Darina Kalina
 */

#include "cvlib.hpp"
#include <vector>
#include <set>
#include <algorithm>

namespace
{
typedef std::vector<cv::Rect> region;

struct ComplexRegion
{
	region rects;
	std::vector<int> neighbours;
	int number_of_pixels;
	bool is_exist;
	double brightness_sum;
	double squares_brightness;
	cv::Rect first_rect;
};

class SplitAndMerge
{
public:
	SplitAndMerge(cv::Mat image);
	void process(cv::Mat& merged, double stddev);

private:
	void split_image(cv::Mat image_for_split, double stddev, cv::Point point);
	bool is_rects_neighbours(const cv::Rect& main_rect, const cv::Rect& possible_neighbour);
	void find_neighbours();
	bool merge_with_neigbours(int region_num, double stddev);
	void merge_image(cv::Mat image_for_merge, double stddev);

    cv::Mat m_splited_image;
	cv::Mat m_merged_image;
	std::vector<ComplexRegion> m_regions;
};

SplitAndMerge::SplitAndMerge(cv::Mat image)
{
	m_regions.clear();
	image.copyTo(m_splited_image);
	m_merged_image = cv::Mat(image.rows, image.cols, image.type(), cv::Scalar(0));
}

void SplitAndMerge::process(cv::Mat& merged, double stddev)
{
	split_image(m_splited_image, stddev, cv::Point(0, 0));
	find_neighbours();
	merge_image(m_merged_image, stddev);
    m_merged_image.copyTo(merged);
}

void SplitAndMerge::split_image(cv::Mat image_for_split, double stddev, cv::Point point)
{
    cv::Mat mean;
    cv::Mat dev;
    cv::meanStdDev(image_for_split, mean, dev);

	const auto width = image_for_split.cols;
	const auto height = image_for_split.rows;

	if ((width == 0) || (height == 0))
		return;

    if (dev.at<double>(0) <= stddev)
    {
		cv::Mat integral;
		cv::Mat square_integral;
		cv::integral(image_for_split, integral, square_integral);
		ComplexRegion region;
		region.first_rect = cv::Rect(point.x, point.y, width, height);
		region.rects.push_back(region.first_rect);
		region.number_of_pixels = width * height;
		region.is_exist = true;

		region.brightness_sum = integral.at<int>(cv::Point(0, 0)) +
			integral.at<int>(cv::Point(width, height)) -
			integral.at<int>(cv::Point(width, 0)) -
			integral.at<int>(cv::Point(0, height));

		region.squares_brightness = square_integral.at<double>(cv::Point(0, 0)) +
			square_integral.at<double>(cv::Point(width, height)) -
			square_integral.at<double>(cv::Point(width, 0)) -
			square_integral.at<double>(cv::Point(0, height));

		m_regions.push_back(region);
		image_for_split.setTo(mean);
        return;
    }

    split_image(image_for_split(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev, point);
    split_image(image_for_split(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev,
		cv::Point(point.x + width / 2, point.y));
    split_image(image_for_split(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev,
		cv::Point(point.x + width / 2, point.y + height / 2));
    split_image(image_for_split(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev,
		cv::Point(point.x, point.y + height / 2));
}

bool SplitAndMerge::is_rects_neighbours(const cv::Rect& main_rect, const cv::Rect& possible_neighbour)
{
	if ((main_rect.x == possible_neighbour.x + possible_neighbour.width) || 
		(main_rect.x + main_rect.width == possible_neighbour.x))
	{
		if ((possible_neighbour.y >= main_rect.y) &&
			(possible_neighbour.y <= main_rect.y + main_rect.height - possible_neighbour.height))
			return true;
	}

	if ((main_rect.y == possible_neighbour.y + possible_neighbour.height) ||
		(main_rect.y + main_rect.height == possible_neighbour.y))
	{
		if ((possible_neighbour.x >= main_rect.x) &&
			(possible_neighbour.x <= main_rect.x + main_rect.width - possible_neighbour.width))
			return true;
	}
	return false;
}

void SplitAndMerge::find_neighbours()
{
	if (m_regions.size() == 1)
		return;

	auto region_size = m_regions.size();
	for (auto i = 0; i < region_size; ++i)
	{
		const cv::Rect& region_i = m_regions.at(i).first_rect;
		auto region_i_area = region_i.area();

		for (auto j = 0; j < region_size; ++j)
		{
			if(i == j)
				continue;

			const cv::Rect& region_j = m_regions.at(j).first_rect;
			auto region_j_area = region_j.area();
			if ((region_i_area >= region_j_area) &&
				is_rects_neighbours(region_i, region_j)
			)
			{
				m_regions[i].neighbours.push_back(j);
				if (region_i_area != region_j_area)
					m_regions[j].neighbours.push_back(i);
			}
		}
	}
}

bool SplitAndMerge::merge_with_neigbours(int region_num, double stddev)
{
	bool result = false;
	for (auto i = 0; i < m_regions[region_num].neighbours.size(); ++i)
	{
		if (!m_regions[m_regions[region_num].neighbours[i]].is_exist)
			continue;

		int number_of_pixels_merged = m_regions[region_num].number_of_pixels +
			m_regions[m_regions[region_num].neighbours[i]].number_of_pixels;

		double brightness_sum_merged = m_regions[region_num].brightness_sum +
			m_regions[m_regions[region_num].neighbours[i]].brightness_sum;

		double squares_brightness_merged = m_regions[region_num].squares_brightness +
			m_regions[m_regions[region_num].neighbours[i]].squares_brightness;

		double square_of_mean = brightness_sum_merged * brightness_sum_merged /
			(number_of_pixels_merged * number_of_pixels_merged);
		double merged_stddev = sqrt((squares_brightness_merged - square_of_mean) /
			number_of_pixels_merged);

		if (merged_stddev <= stddev)
		{
			result = true;
			m_regions[region_num].number_of_pixels = number_of_pixels_merged;
			m_regions[region_num].brightness_sum = brightness_sum_merged;
			m_regions[region_num].squares_brightness = squares_brightness_merged;
			m_regions[m_regions[region_num].neighbours[i]].is_exist = false;

			for (auto& new_rect : m_regions[m_regions[region_num].neighbours[i]].rects)
				m_regions[region_num].rects.push_back(new_rect);

			for (auto& new_neighbour :
				m_regions[m_regions[region_num].neighbours[i]].neighbours)
			{
				if (new_neighbour != region_num)
					m_regions[region_num].neighbours.push_back(new_neighbour);
			}
			m_regions[m_regions[region_num].neighbours[i]].neighbours.clear();
			m_regions[m_regions[region_num].neighbours[i]].rects.clear();
		}
	}

	return result;
}

void SplitAndMerge::merge_image(cv::Mat image_for_merge, double stddev)
{
	bool next_iter_required = true;
	while (next_iter_required)
	{
		next_iter_required = false;
		for (auto region_num = 0; region_num < m_regions.size(); ++region_num)
		{
			if (m_regions[region_num].is_exist)
			{
				next_iter_required |= merge_with_neigbours(region_num, stddev);
			}
		}

		for (const auto& region : m_regions)
		{
			if (region.is_exist)
			{
				unsigned int mean_value = static_cast<unsigned int>(region.brightness_sum /
					region.number_of_pixels);

				for (const auto& rect : region.rects)
					cv::rectangle(image_for_merge, rect, cv::Scalar(mean_value), -1);
			}
		}
	}
}

} // namespace

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev)
{
    cv::Mat res = image;
	cv::Mat merged;
	SplitAndMerge split_merge(res);
	split_merge.process(merged, stddev);

	return merged;
}
} // namespace cvlib
