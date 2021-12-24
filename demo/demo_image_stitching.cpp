/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-11-25
 * @author Anonymous
 */

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

#include "utils.hpp"

int demo_image_stitching(int argc, char* argv[])
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

    const auto main_wnd = "orig";
    const auto demo_wnd = "demo";

    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);

    //auto detector = cvlib::corner_detector_fast().create();
    auto detector = cv::AKAZE::create();
    auto matcher = cvlib::descriptor_matcher(1.2f); //\todo add trackbar to demo_wnd to tune ratio value

    struct img_features
    {
        cv::Mat img;
        std::vector<cv::KeyPoint> corners;
        cv::Mat descriptors;
    };

    img_features ref;
    img_features test;
    std::vector<std::vector<cv::DMatch>> pairs;

    cv::Mat main_frame;
    cv::Mat demo_frame;
    utils::fps_counter fps;
    int pressed_key = 0;

    int ratio = 8;
    int max_distance = 50000;
    int corner_thresh = 10;


    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;

//    auto stitcher = cv::Stitcher::create();
//    stitcher->setFeaturesFinder(detector);
    auto stitcher = cvlib::stitcher::create();

    while (pressed_key != 27) // ESC
    {
        cap >> test.img;

        //detector->thresh = corner_thresh;
//        detector->detect(test.img, test.corners);
//        cv::drawKeypoints(test.img, test.corners, main_frame);
        cv::imshow(main_wnd, test.img);
//
        pressed_key = cv::waitKey(30);
        if (pressed_key == ' ') // space
        {
            ref.img = test.img.clone();
//            //cv::flip( ref.img,  ref.img, 0);
//            detector->detectAndCompute(ref.img, cv::Mat(), ref.corners, ref.descriptors);
        }
//
//        if (ref.corners.empty())
//        {
//            continue;
//        }
//
//        matcher.set_ratio(ratio / 10.0f);
//        detector->compute(test.img, test.corners, test.descriptors);
//
//        matcher.radiusMatch(test.descriptors, ref.descriptors, pairs, max_distance / 1.0f);
//
//        std::vector<cv::DMatch> good_matches;
//        for (int i = 0; i < pairs.size(); ++i)
//        {
//            if (pairs[i].size())
//                good_matches.push_back((pairs[i][0]));
//        }
//        for (size_t i = 0; i < good_matches.size(); i++)
//        {
//            obj.push_back(test.corners[good_matches[i].queryIdx].pt);
//            scene.push_back(ref.corners[good_matches[i].trainIdx].pt);
//        }
//
//        cv::Mat h = findHomography(scene, obj, cv::RANSAC);
//
//        cv::drawMatches(test.img, test.corners, ref.img, ref.corners, pairs, demo_frame);
//        cv::Mat im_out;
//        warpPerspective(test.img, im_out, h, cv::Size(test.img.cols*2,test.img.rows));
//        // cv::Mat result23;
//
//        cv::Mat half(im_out,cv::Rect(0,0,test.img.cols,test.img.rows));
//        ref.img.copyTo(half);
//
//
//
//        cv::imshow(test_wnd, im_out);
//
//
//
//        utils::put_fps_text(demo_frame, fps);
//
        if(!ref.img.empty())
        {
            auto result = stitcher->stiched(test.img,ref.img);
            cv::imshow(demo_wnd, result);
        }
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);
    return 0;
}
