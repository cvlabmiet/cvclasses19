/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <deque>

#include <cvlib.hpp>
#include <opencv2/opencv.hpp>

struct FramesBuffer
{
    std::deque<cv::Mat> frames;
    size_t max_size;

    public:
    FramesBuffer(size_t max_size) : max_size(max_size)
    {
    }

    void push_frame(cv::Mat frame)
    {
        if (frames.size() == max_size)
            frames.pop_front();
        frames.push_back(frame);
    }

    cv::Mat mean()
    {
        cv::Mat mean = cv::Mat(frames.front().size(), CV_32FC3, cv::Scalar());
        for (auto& frame : frames)
            cv::accumulate(frame, mean);
        mean.convertTo(mean, CV_8UC3, 1.0 / frames.size());
        return mean;
    }
};

int demo_motion_segmentation(int argc, char* argv[])
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;

    static const auto buff_size = 10;
    auto buffer = FramesBuffer(buff_size);
    for (auto idx = 0; idx < buff_size; ++idx)
    {
        cv::Mat frame;
        cap >> frame;
        buffer.push_frame(frame);
    }

    auto mseg = cvlib::motion_segmentation(buffer.mean());
    const auto main_wnd = "orig";
    const auto demo_wnd = "demo";

    cv::namedWindow(main_wnd);
    cv::namedWindow(demo_wnd);

    int rate_track = static_cast<int>(1000 * 1.0 / buff_size);
    cv::createTrackbar("rate", demo_wnd, &rate_track, 1000);
    double rate = static_cast<double>(rate_track) / 1000;

    cv::Mat frame;
    cv::Mat frame_mseg;
    static const auto ESC_KEY_CODE = 27;
    while (cv::waitKey(30) != ESC_KEY_CODE)
    {
        cap >> frame;
        cv::imshow(main_wnd, frame);
        mseg.apply(frame, frame_mseg, rate);
        if (!frame_mseg.empty())
            cv::imshow(demo_wnd, frame_mseg);
    }

    cv::destroyWindow(main_wnd);
    cv::destroyWindow(demo_wnd);

    return 0;
}
