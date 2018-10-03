/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-18
 * @author Darina Kalina
 */

#include "cvlib.hpp"

#define SIGMA_SZ 3
#define THETA_SZ 2

namespace
{
struct descriptor : public std::vector<double>
{
    using std::vector<double>::vector;
    descriptor operator-(const descriptor& right) const
    {
        descriptor temp = *this;
        for (int i = 0; i < temp.size(); ++i)
        {
            temp[i] -= right[i];
        }
        return temp;
    }

    double norm_l1() const
    {
        double res = 0.0;
        for (auto v : *this)
        {
            res += std::abs(v);
        }
        return res;
    }

	double norm_l2() const
	{
		double res = 0.0;
		for (auto v : *this)
		{
			res += v * v;
		}
		return sqrt(res);
	}
};

double calculate_sum_from_intgral(const cv::Mat& integral, const cv::Rect& rect)
{
	return integral.at<double>(rect.y, rect.x) + integral.at<double>(rect.y + rect.height, rect.x + rect.width) -
		integral.at<double>(rect.y, rect.x + rect.width) - integral.at<double>(rect.y + rect.height, rect.x);
}

struct GaborFiltersParams
{
	double sigma[SIGMA_SZ];
	double theta[THETA_SZ];
	double lambda;
	double gamma;
};

class GaborTextureSegmentationDescr
{
public:
	GaborTextureSegmentationDescr(const cv::Mat& image, const cv::Rect& roi,
		const GaborFiltersParams& params);
	double calculate_similarity_measure(const cv::Mat& image, const cv::Rect& roi);
private:
	int calculate_kernel_size(const cv::Rect& roi);
	void calculate_integrals(const cv::Mat& image, const cv::Rect& roi);
	void calculate_descriptor(const cv::Rect& roi, descriptor& descr);

	const GaborFiltersParams& m_params;

	std::vector<cv::Mat> m_integral_images;
	std::vector<cv::Mat> m_squares_integral_images;
	descriptor m_reference_descriptor;
};

GaborTextureSegmentationDescr::GaborTextureSegmentationDescr(const cv::Mat& image, const cv::Rect& roi,
	const GaborFiltersParams& params) : m_params(params)
{
	calculate_integrals(image, roi);
	calculate_descriptor(roi, m_reference_descriptor);
}

double GaborTextureSegmentationDescr::calculate_similarity_measure(const cv::Mat& image, const cv::Rect& roi)
{
	descriptor current_descriptor;
	calculate_descriptor(roi, current_descriptor);
	return (current_descriptor - m_reference_descriptor).norm_l2();
}

int GaborTextureSegmentationDescr::calculate_kernel_size(const cv::Rect& roi)
{
	int kernel_size = std::min(roi.width, roi.height) / 2;
	if (kernel_size % 2 == 0)
		kernel_size == 0 ? ++kernel_size : --kernel_size;
	return kernel_size;
}

void GaborTextureSegmentationDescr::calculate_integrals(const cv::Mat& image, const cv::Rect& roi)
{
	cv::Mat filtered;
	cv::Mat integral_filtered;
	cv::Mat square_integral_filtered;

	int kernel_size = calculate_kernel_size(roi);

	for (auto i = 0; i < THETA_SZ; i++)
	{
		for (auto j = 0; j < SIGMA_SZ; j++)
		{
			cv::Mat kernel = cv::getGaborKernel(cv::Size(kernel_size, kernel_size),
				m_params.sigma[i], m_params.sigma[j], m_params.lambda, m_params.gamma);
			cv::filter2D(image, filtered, CV_64F, kernel);

			cv::integral(filtered, integral_filtered, square_integral_filtered);

			m_integral_images.push_back(integral_filtered);
			m_squares_integral_images.push_back(square_integral_filtered);
		}
	}
}

void GaborTextureSegmentationDescr::calculate_descriptor(const cv::Rect& roi, descriptor& descr)
{
	descr.clear();
	int kernel_size = calculate_kernel_size(roi);
	double mean = 0.0;
	double square_mean = 0.0;
	double stddev = 0.0;

	size_t pixels_number = roi.area();

	for (size_t i = 0; i < m_integral_images.size(); i++)
	{
		mean = calculate_sum_from_intgral(m_integral_images[i], roi) / pixels_number;
		descr.emplace_back(mean);

		square_mean = calculate_sum_from_intgral(m_squares_integral_images[i], roi) / pixels_number;
		stddev = std::sqrt(square_mean - mean * mean);
		descr.emplace_back(stddev);
	}
}
} // namespace

namespace cvlib
{
cv::Mat select_texture(const cv::Mat& image, const cv::Rect& roi, double eps)
{
	if (image.empty())
		return image;

	GaborFiltersParams params;
	params.gamma = 0.5;
	params.lambda = 10;
	params.theta[0] = CV_PI / 4;
	params.theta[1] = -CV_PI / 4;
	params.sigma[0] = 5;
	params.sigma[1] = 10;
	params.sigma[2] = 15;

	GaborTextureSegmentationDescr descriptor(image, roi, params);

	cv::Mat res = cv::Mat::zeros(image.size(), CV_8UC1);
	cv::Rect main_roi = roi - roi.tl();
	cv::Rect current_roi;
	double similarity_measure = 0.0;

	for (size_t i = 0; i <= image.rows - roi.height; i++)
	{
		for (size_t j = 0; j <= image.cols - roi.width; j++)
		{
			current_roi = main_roi + cv::Point(j, i);
			similarity_measure = descriptor.calculate_similarity_measure(image, current_roi);
			res(current_roi) += 255 * (similarity_measure <= eps);
		}
	}
    return res;
}
} // namespace cvlib
