/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"

namespace {
void split_image(cv::Mat image, double stddev) {
    cv::Mat mean;
    cv::Mat dev;
    cv::meanStdDev(image, mean, dev);

    if (dev.at<double>(0) <= stddev) {
        image.setTo(mean);
        return;
    }

    const auto width = image.cols;
    const auto height = image.rows;

    split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev);
    split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)),
                stddev);
    split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)),
                stddev);
    split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)),
                stddev);
}
} // namespace

namespace {
const unsigned diffMean = 10;
void merge_image(cv::Mat image, double stddev) {
    cv::Mat mean;
    cv::Mat dev;
    cv::meanStdDev(image, mean, dev);

    if (dev.at<double>(0) <= stddev) {
        image.setTo(mean);
        return;
    }

    cv::Mat Rmean_0;
    cv::Mat Rdev_0;
    cv::Mat Rmean_1;
    cv::Mat Rdev_1;
    cv::Mat Rmean_2;
    cv::Mat Rdev_2;
    cv::Mat Rmean_3;
    cv::Mat Rdev_3;

    const auto width = image.cols;
    const auto height = image.rows;

    cv::Mat R_0 = image(cv::Range(0, height / 2), cv::Range(0, width / 2));

    cv::Mat R_1 = image(cv::Range(0, height / 2), cv::Range(width / 2, width));

    cv::Mat R_2 = image(cv::Range(height / 2, height), cv::Range(0, width / 2));

    cv::Mat R_3 =
        image(cv::Range(height / 2, height), cv::Range(width / 2, width));

    cv::meanStdDev(R_0, Rmean_0, Rdev_0);
    cv::meanStdDev(R_1, Rmean_1, Rdev_1);
    cv::meanStdDev(R_2, Rmean_2, Rdev_2);
    cv::meanStdDev(R_3, Rmean_3, Rdev_3);

    int flag_0 = 0;
    int flag_1 = 0;
    int flag_2 = 0;
    int flag_3 = 0;
    if (Rdev_0.at<double>(0) <= stddev && Rdev_1.at<double>(0) <= stddev &&
            abs(Rmean_0.at<double>(0) - Rmean_1.at<double>(0)) <= diffMean) {
        flag_0 = 1;
        R_0.setTo((Rmean_0 + Rmean_1) / 2);
        R_1.setTo((Rmean_0 + Rmean_1) / 2);
    }

    if (Rdev_3.at<double>(0) <= stddev && Rdev_1.at<double>(0) <= stddev &&
            !flag_0 &&
            abs(Rmean_3.at<double>(0) - Rmean_1.at<double>(0)) <= diffMean) {
        flag_2 = 1;
        R_3.setTo((Rmean_3 + Rmean_1) / 2);
        R_1.setTo((Rmean_3 + Rmean_1) / 2);
    }

    if (Rdev_3.at<double>(0) <= stddev && Rdev_2.at<double>(0) <= stddev &&
            !flag_2 &&
            abs(Rmean_3.at<double>(0) - Rmean_2.at<double>(0)) <= diffMean) {
        flag_3 = 1;
        R_2.setTo((Rmean_3 + Rmean_2) / 2);
        R_3.setTo((Rmean_3 + Rmean_2) / 2);
    }

    if (Rdev_0.at<double>(0) <= stddev && Rdev_2.at<double>(0) <= stddev &&
            !flag_0 && !flag_3 &&
            abs(Rmean_0.at<double>(0) - Rmean_2.at<double>(0)) <= diffMean) {
        flag_1 = 1;
        R_0.setTo((Rmean_0 + Rmean_2) / 2);
        R_2.setTo((Rmean_0 + Rmean_2) / 2);
    }

    if (!flag_0 && !flag_1)
        merge_image(R_0, stddev);

    if (!flag_0 && !flag_2)
        merge_image(R_1, stddev);

    if (!flag_1 && !flag_3)
        merge_image(R_2, stddev);

    if (!flag_2 && !flag_3)
        merge_image(R_3, stddev);
}
} // namespace

namespace cvlib {
cv::Mat split_and_merge(const cv::Mat &image, double stddev) {
    // split part
    cv::Mat res = image;
    split_image(res, stddev);

    // merge part
    // \todo implement merge algorithm
    return res;
}
} // namespace cvlib

namespace cvlib {
cv::Mat cvlib_split(const cv::Mat &image, double stddev) {
    cv::Mat res = image;
    split_image(res, stddev);
    return res;
}
} // namespace cvlib

namespace cvlib {
cv::Mat cvlib_merge(const cv::Mat &image, double stddev) {
    cv::Mat res = image;
    merge_image(res, stddev);
    return res;
}
} // namespace cvlib
