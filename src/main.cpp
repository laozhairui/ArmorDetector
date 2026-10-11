
#include <video_capture.h>
#include <color_detector.h>

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

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
    cv::Mat display;

    std::string select = "blue";
    std::string mask_window = select + " Mask";

    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::resizeWindow("Original", 640, 480);
    cv::moveWindow("Original", 0, 0);

    create_mask_window(mask_window);

    while (reader.read_frame(frame))
    {
        detector.detect(frame, select, select_mask);

        display = frame.clone();

        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;

        cv::Mat contour_input = select_mask.clone();

        cv::findContours(
            contour_input,
            contours,
            hierarchy,
            cv::RETR_EXTERNAL,
            cv::CHAIN_APPROX_SIMPLE
        );

        int valid_contour_count = 0;

        for (int i = 0; i < static_cast<int>(contours.size()); i++)
        {
            double area = cv::contourArea(contours[i]);

            if (area < 50)
            {
                continue;
            }

            cv::drawContours(
                display,
                contours,
                i,
                cv::Scalar(0, 255, 0),
                2
            );

            valid_contour_count++;
        }

        cv::putText(
            display,
            "Contours: " + std::to_string(valid_contour_count),
            cv::Point(20, 35),
            cv::FONT_HERSHEY_SIMPLEX,
            0.8,
            cv::Scalar(0, 255, 0),
            2
        );

        cv::imshow("Original", display);
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
