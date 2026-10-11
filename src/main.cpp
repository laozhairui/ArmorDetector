#include <video_capture.h>
#include <color_detector.h>
#include <light_bar.h>

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


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


        cv::findContours(
            select_mask.clone(),
            contours,
            hierarchy,
            cv::RETR_EXTERNAL,
            cv::CHAIN_APPROX_SIMPLE
        );


        std::vector<LightBar> light_bars;


        for (int i = 0; i < static_cast<int>(contours.size()); i++)
        {
            double area = cv::contourArea(contours[i]);


            if (area < 10)
            {
                continue;
            }


            cv::RotatedRect rect = cv::minAreaRect(contours[i]);


            double width = rect.size.width;
            double height = rect.size.height;


            double long_side = std::max(width, height);
            double short_side = std::min(width, height);


            if (short_side <= 0)
            {
                continue;
            }


            double aspect_ratio = long_side / short_side;


            if (aspect_ratio < 2.0)
            {
                continue;
            }


            LightBar bar;

            bar.rect = rect;
            bar.length = long_side;
            bar.width = short_side;
            bar.angle = rect.angle;


            light_bars.push_back(bar);
        }


        for (const auto& bar : light_bars)
        {
            cv::Point2f vertices[4];

            bar.rect.points(vertices);


            for (int j = 0; j < 4; j++)
            {
                cv::line(
                    display,
                    vertices[j],
                    vertices[(j + 1) % 4],
                    cv::Scalar(0, 255, 0),
                    2
                );
            }
        }


        cv::putText(
            display,
            "LightBars: " + std::to_string(light_bars.size()),
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