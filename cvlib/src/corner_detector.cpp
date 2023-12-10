/* FAST corner detector algorithm implementation.
 * @file
 * @date 2023-12-10
 * @author Yaroslav Murenkov
 */

#include "cvlib.hpp"

#include <ctime>
#include <random>
#include <vector>

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

    cv::Mat img;
    image.getMat().copyTo(img);
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

void corner_detector_fast::compute(cv::InputArray image, std::vector<cv::KeyPoint>& keypoints, cv::OutputArray descriptors)
{
    cv::Mat img;
    image.getMat().copyTo(img);
    if (img.channels() == 3)
        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(img, img, cv::Size(5, 5), 2, 2);

    const auto neighbourhood_size = 25;
    const auto neighbourhood_halfsize = neighbourhood_size / 2 + 1;

    const auto desc_length = 32;
    descriptors.create(static_cast<int>(keypoints.size()), desc_length, CV_8U);
    auto desc_mat = descriptors.getMat();
    desc_mat.setTo(0);

    auto random_pairs = [](auto size, auto desc_length) {
        const auto halfsize = size / 2;
        std::default_random_engine randgen;
        std::normal_distribution<double> norm(0, size / 2);

        auto randint = [&randgen, &norm](auto left, auto right) { return std::clamp(static_cast<int>(std::round(norm(randgen))), left, right); };

        std::vector<std::pair<cv::Point, cv::Point>> pairs;
        for (int i = 0; i < desc_length; i++)
            pairs.push_back(std::make_pair(cv::Point(randint(-halfsize, halfsize), randint(-halfsize, halfsize)),
                                           cv::Point(randint(-halfsize, halfsize), randint(-halfsize, halfsize))));

        return pairs;
    };
    auto test = [&img](cv::Point kp, std::pair<cv::Point, cv::Point> p) { return img.at<uint8_t>(kp + p.first) < img.at<uint8_t>(kp + p.second); };

    auto ptr = reinterpret_cast<uint8_t*>(desc_mat.ptr());
    for (const auto& kp : keypoints)
    {
        for (int i = 0; i < desc_length; ++i)
        {
            uint8_t descriptor = 0;
            if (this->pairs.empty())
                this->pairs = random_pairs(neighbourhood_size, desc_length);
            for (auto j = 0; j < pairs.size(); ++j)
                descriptor |= (test(kp.pt, pairs.at(j)) << (pairs.size() - 1 - j));
            *ptr = descriptor;
            ++ptr;
        }
    }
}

void corner_detector_fast::detectAndCompute(cv::InputArray, cv::InputArray, std::vector<cv::KeyPoint>&, cv::OutputArray descriptors, bool /*= false*/)
{
    // \todo implement me
}
} // namespace cvlib
