/* Descriptor matcher algorithm implementation.
 * @file
 * @date 2018-11-25
 * @author Anonymous
 */

#include "cvlib.hpp"

namespace cvlib
{
void descriptor_matcher::knnMatchImpl(cv::InputArray queryDescriptors, std::vector<std::vector<cv::DMatch>>& matches, int k /*unhandled*/,
                                      cv::InputArrayOfArrays masks /*unhandled*/, bool compactResult /*unhandled*/)
{
    if (trainDescCollection.empty())
        return;

    auto q_desc = queryDescriptors.getMat();
    auto& t_desc = trainDescCollection[0];

    matches.resize(q_desc.rows);

    auto distance = [](const cv::Mat q_desc, const cv::Mat t_desc) {
        auto dist = 0;
        for (auto i = 0; i < q_desc.cols; ++i) {
            const auto q_desc_b = q_desc.at<uint8_t>(0, i);
            const auto t_desc_b = q_desc.at<uint8_t>(0, i);

            for (auto diff = q_desc_b ^ t_desc_b; diff > 0; diff >>= 1)
                ++dist;
        }
        return dist;
    };

    cv::RNG rnd;
    for (int i = 0; i < q_desc.rows; ++i)
    {
        for (auto j = 0; j < t_desc.rows; ++j)
        {
            auto dist = distance(q_desc.row(i), t_desc.row(i));
            if (dist < this->ratio_)
                matches[i].emplace_back(i, j, dist);
        }
    }
}

void descriptor_matcher::radiusMatchImpl(cv::InputArray queryDescriptors, std::vector<std::vector<cv::DMatch>>& matches, float /*maxDistance*/,
                                         cv::InputArrayOfArrays masks /*unhandled*/, bool compactResult /*unhandled*/)
{
    // \todo implement matching with "maxDistance"
    knnMatchImpl(queryDescriptors, matches, 1, masks, compactResult);
}
} // namespace cvlib
