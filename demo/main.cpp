/* Demo application for Computer Vision Library.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include <opencv2/opencv.hpp>

int project_markup(int argc, char* argv[]); // markup_application
int demo_split_and_merge(int argc, char* argv[]); // lab 1
int demo_select_texture(int argc, char* argv[]); // lab 2
int demo_motion_segmentation(int argc, char* argv[]); // lab 3
int demo_corner_detector(int argc, char* argv[]); // lab 4
int demo_feature_descriptor(int argc, char* argv[]); // lab 5
int demo_feature_matching(int argc, char* argv[]); // lab 6
int demo_image_stitching(int argc, char* argv[]); // lab 7

int main(int argc, char* argv[])
{
    cv::namedWindow("main");
    cv::Mat help = cv::Mat::zeros(300, 500, CV_8UC3);

    // clang-format off
    auto putText = [help, x = 30](const std::string& text) mutable {
        cv::putText(help, text, cv::Point(30, x), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
        x += 20;
    };
    // clang-format on

    putText("Press ESC to exit");
    putText("Press 1 for Lab 1 Demo (split and merge)");
    putText("Press 2 for Lab 2 Demo (texture segmentation)");
    putText("Press 3 for Lab 3 Demo (motion segmentation)");
    putText("Press 4 for Lab 4 Demo (corner detector)");
    putText("Press 5 for Lab 5 Demo (feature descriptor)");
    putText("Press 6 for Lab 6 Demo (feature matching)");
    putText("Press 7 for Lab 7 Demo (image stitching)");
    putText("--------------------------------------------");
    putText("Press M for markup video file (course project tool)");

    cv::imshow("main", help);

    while (true)
    {
        int key = cv::waitKey(0);

        switch (key)
        {
            case '1':
                demo_split_and_merge(argc, argv);
                break;
            case '2':
                demo_select_texture(argc, argv);
                break;
            case '3':
                demo_motion_segmentation(argc, argv);
                break;
            case '4':
                demo_corner_detector(argc, argv);
                break;
            case '5':
                demo_feature_descriptor(argc, argv);
                break;
            case '6':
                demo_feature_matching(argc, argv);
                break;
            case '7':
                demo_image_stitching(argc, argv);
                break;
            case 'M':
            case 'm':
                project_markup(argc, argv);
                break;
            case 27: // ESC
                return 0;
        }
    }
}
