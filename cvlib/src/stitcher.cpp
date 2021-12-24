//
// Created by viktor on 12/15/21.
//
#include "cvlib.hpp"
namespace cvlib
{
cv::Mat stitcher::stiched(cv::Mat img1, cv::Mat img2)
{
    auto detector = cv::AKAZE::create();
    auto matcher = cvlib::descriptor_matcher(1.2f);
    struct img_features
    {
        cv::Mat img;
        std::vector<cv::KeyPoint> corners;
        cv::Mat descriptors;
    };

    img_features ref;
    img_features test;
    test.img = img1.clone();
    ref.img = img2.clone();
    std::vector<std::vector<cv::DMatch>> pairs;

    detector->detectAndCompute(test.img, cv::Mat(), test.corners, test.descriptors);
    detector->detectAndCompute(ref.img, cv::Mat(), ref.corners, ref.descriptors);
    matcher.set_ratio(0.7f);
    matcher.radiusMatch(test.descriptors, ref.descriptors, pairs, 50000);

    std::vector<cv::Point2f> obj, scene;
    std::vector<cv::DMatch> good_matches;
    for (int i = 0; i < pairs.size(); ++i)
    {
        if (pairs[i].size())
            good_matches.push_back((pairs[i][0]));
    }
    for (size_t i = 0; i < good_matches.size(); i++)
    {
        obj.push_back(test.corners[good_matches[i].queryIdx].pt);
        scene.push_back(ref.corners[good_matches[i].trainIdx].pt);
    }

    cv::Rect croppImg1(0, 0, img1.cols, img1.rows);
    cv::Rect croppImg2(0, 0, img2.cols, img2.rows);
    int movementDirection = 0;
    int imgWidth = img1.cols;
    for (int i = 0; i < obj.size(); ++i)
    {
        if (obj[i].x < imgWidth)
        {
            croppImg1.width = obj.at(i).x;
            croppImg2.x = scene[i].x;
            croppImg2.width = img2.cols - croppImg2.x;
            movementDirection = obj[i].y - scene[i].y;
            imgWidth = obj[i].x;
        }
    }
    img1 = img1(croppImg1);
    img2 = img2(croppImg2);
    int maxHeight = img1.rows > img2.rows ? img1.rows : img2.rows;
    int maxWidth = img1.cols + img2.cols;
    cv::Mat result=cv::Mat::zeros(cv::Size(maxWidth, maxHeight + abs(movementDirection)), CV_8UC3);
    if (movementDirection > 0)
    {
        cv::Mat half1(result, cv::Rect(0, 0, img1.cols, img1.rows));
        img1.copyTo(half1);
        cv::Mat half2(result, cv::Rect(img1.cols, abs(movementDirection),img2.cols, img2.rows));
        img2.copyTo(half2);
    }
    else
    {
        cv::Mat half1(result, cv::Rect(0, abs(movementDirection), img1.cols, img1.rows));
        img1.copyTo(half1);
        cv::Mat half2(result, cv::Rect(img1.cols,0 ,img2.cols, img2.rows));
        img2.copyTo(half2);
    }
    return result;
}
};