#include <video_capture.h>
#include <color_detector.h>

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

void create_mask_window(const std::string& name)
{
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    cv::resizeWindow(name, 640, 480);
    cv::moveWindow(name, 800, 0);
}

int main()
{
    VideoCapture reader("data/test.mp4");

    if (!reader.is_opened())
    {
        std::cerr << "Failed to open video." << std::endl;
        return -1;
    }

    ColorDetecter detector;

    cv::Mat frame;
    cv::Mat select_mask;

    std::string select = "blue";
    std::string mask_window = select + " Mask";

    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::resizeWindow("Original", 640, 480);
    cv::moveWindow("Original", 0, 0);

    create_mask_window(mask_window);

    while (reader.read_frame(frame))
    {
        detector.detect(frame, select, select_mask);

        cv::imshow("Original", frame);
        cv::imshow(mask_window, select_mask);

        int key = cv::waitKey(1);

        if (key == 27)
        {
            break;
        }
        else if ((key == 'r' || key == 'R') && select != "red")
        {
            cv::destroyWindow(mask_window);
            select = "red";
            mask_window = select + " Mask";
            create_mask_window(mask_window);
        }
        else if ((key == 'b' || key == 'B') && select != "blue")
        {
            cv::destroyWindow(mask_window);
            select = "blue";
            mask_window = select + " Mask";
            create_mask_window(mask_window);
        }
    }

    cv::destroyAllWindows();

    return 0;
}
