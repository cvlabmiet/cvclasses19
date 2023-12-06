/* Demo application for Computer Vision Library.
 * @file
 * @date 2023-10-25
 * @author Yaroslav Murenkov
 */

#include <opencv2/opencv.hpp>

#include <cvlib.hpp>

namespace
{
struct user_data
{
    std::string wnd;
    cv::Point tl;
    cv::Point br;
    cv::Mat image;
};

void mouse(int event, int x, int y, int flags, void* param)
{
    user_data& data = *reinterpret_cast<user_data*>(param);
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        data.tl = {x, y};
    }
    else if (event == cv::EVENT_RBUTTONDOWN)
    {
        data.br = {x, y};
    }
}
} // namespace

int demo_select_texture(int argc, char* argv[])
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

    user_data data;

    data.wnd = "origin";
    const auto demo_wnd = "demo";

    int eps = 50;
    cv::namedWindow(data.wnd);
    cv::namedWindow(demo_wnd);
    cv::createTrackbar("eps", demo_wnd, &eps, 110);

    cv::setMouseCallback(data.wnd, mouse, &data);

    cv::Mat frame_gray;
    const auto ESC_KEY_CODE = 27;
    while (cv::waitKey(30) != ESC_KEY_CODE)
    {
        cap >> data.image;

        cv::cvtColor(data.image, frame_gray, cv::COLOR_BGR2GRAY);
        const cv::Rect roi = {data.tl, data.br};
        if (roi.area())
        {
            const auto mask = cvlib::select_texture(frame_gray, roi, eps);
            const auto segmented = mask.clone();
            frame_gray.copyTo(segmented, mask);
            cv::imshow(demo_wnd, segmented);
            cv::rectangle(data.image, data.tl, data.br, cv::Scalar(0, 0, 255));
        }
        cv::imshow(data.wnd, data.image);
    }

    cv::destroyWindow(data.wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
