/* FAST corner detector algorithm implementation.
 * @file
 * @date 2018-10-16
 * @author Anonymous
 */

#include "cvlib.hpp"

#include <ctime>

namespace cvlib
{
// static
cv::Ptr<corner_detector_fast> corner_detector_fast::create()
{
    return cv::makePtr<corner_detector_fast>();
}

void corner_detector_fast::detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray /*mask = cv::noArray()*/)
{
    keypoints.clear();

    const auto img_ = image.getMat();
    cv::Mat img;
    img_.copyTo(img);
    if (img.channels() == 3)
        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

    const auto threshold = 5;
    const auto [fast_test, slow_test] = std::make_pair(3, 12);
    // clang-format off
    const auto offset = std::vector<cv::Point>{
		cv::Point(-3,  0), cv::Point(-3,  1), cv::Point(-2,  2), cv::Point(-1,  3),
		cv::Point( 0,  3), cv::Point( 1,  3), cv::Point( 2,  2), cv::Point( 3,  1),
		cv::Point( 3,  0), cv::Point( 3, -1), cv::Point( 2, -2), cv::Point( 1, -3),
        cv::Point( 0, -3), cv::Point(-1, -3), cv::Point(-2, -2), cv::Point(-3, -1)
	}; // clang-format on

    auto count_pixels = [&img, &offset](cv::Point center_coord, size_t step = 1) {
        auto get = [&img](cv::Point center, cv::Point offset = cv::Point(0, 0)) { return img.at<uint8_t>(center + offset); };
        auto darker = [](uint8_t test, uint8_t center, uint8_t thresh) { return test < center - threshold; };
        auto lighter = [](uint8_t test, uint8_t center, uint8_t thresh) { return test > center + threshold; };
        auto [counter, counter_max] = std::make_pair(0, 0);
        for (auto idx = 1; idx < 2 * offset.size(); idx += step)
        {
            const auto center = get(center_coord);

            const auto bord_prev = get(center_coord, offset.at((idx - 1) % offset.size()));
            const auto bord_curr = get(center_coord, offset.at(idx % offset.size()));

            const auto both_darker = darker(bord_prev, center, threshold) && darker(bord_curr, center, threshold);
            const auto both_lighter = lighter(bord_prev, center, threshold) && lighter(bord_curr, center, threshold);
            const auto same_threshed = both_darker || both_lighter;

            counter = same_threshed ? counter + 1 : 0;
            if (counter > counter_max)
                counter_max = counter;
        }
        return counter_max;
    };

    for (auto row = 3; row < img.rows - 3; ++row)
    {
        for (auto col = 3; col < img.cols - 3; ++col)
        {
            const auto center_coord = cv::Point(col, row);
            if (count_pixels(center_coord, 4) < fast_test)
                continue;
            if (count_pixels(center_coord) >= slow_test)
                keypoints.push_back(cv::KeyPoint(center_coord, 3));
        }
    }
}

void corner_detector_fast::compute(cv::InputArray, std::vector<cv::KeyPoint>& keypoints, cv::OutputArray descriptors)
{
    std::srand(unsigned(std::time(0))); // \todo remove me
                                        // \todo implement any binary descriptor
    const int desc_length = 2;
    descriptors.create(static_cast<int>(keypoints.size()), desc_length, CV_32S);
    auto desc_mat = descriptors.getMat();
    desc_mat.setTo(0);

    int* ptr = reinterpret_cast<int*>(desc_mat.ptr());
    for (const auto& pt : keypoints)
    {
        for (int i = 0; i < desc_length; ++i)
        {
            *ptr = std::rand();
            ++ptr;
        }
    }
}

void corner_detector_fast::detectAndCompute(cv::InputArray, cv::InputArray, std::vector<cv::KeyPoint>&, cv::OutputArray descriptors, bool /*= false*/)
{
    // \todo implement me
}
} // namespace cvlib
