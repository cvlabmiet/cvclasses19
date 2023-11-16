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

    auto img = image.getMat();
    if (img.channels() == 3)
        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

    const auto threshold = 10;
    const auto max_in_row = 12;
    // clang-format off
    const auto offset = std::vector<cv::Point>{
		cv::Point(-3,  0), cv::Point(-3,  1), cv::Point(-2,  2), cv::Point(-1,  3),
		cv::Point( 0,  3), cv::Point( 1,  3), cv::Point( 2,  2), cv::Point( 3,  1), 
		cv::Point( 3,  0), cv::Point( 3, -1), cv::Point( 2, -2), cv::Point( 1, -3),
        cv::Point( 0, -3), cv::Point(-1, -3), cv::Point(-2, -2), cv::Point(-3, -1)
	}; // clang-format on

    auto get = [&img](cv::Point center, cv::Point offset = cv::Point(0, 0)) { return img.at<uint8_t>(center + offset); };
    auto in = [](uint8_t test, uint8_t center, uint8_t thresh) { return (test > center - threshold) && (test < center + threshold); };

    for (auto row = 3; row < img.rows - 3; ++row)
    {
        for (auto col = 3; col < img.cols - 3; ++col)
        {
            const auto curr = cv::Point(col, row);

            auto counter = 0;
            for (auto idx = 0; idx < offset.size(); idx += 4)
                if (!in(get(curr, offset.at(idx)), get(curr), threshold))
                    ++counter;
            if (counter < 3)
                continue;

            counter = 0;
            for (auto idx = 0; idx < offset.size(); ++idx)
                counter = (!in(get(curr, offset.at(idx)), get(curr), threshold)) ? counter + 1 : 0;
            if (counter < max_in_row)
                keypoints.push_back(cv::KeyPoint(curr, 3));
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
