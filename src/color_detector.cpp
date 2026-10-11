#include <color_detector.h>

void ColorDetecter::detect(
    const cv::Mat& frame,
    const std::string& select,
    cv::Mat& select_mask
)
{
    cv::Mat hsv;

    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    if (select == "red")
    {
        cv::inRange(
            hsv,
            m_lower_red,
            m_upper_red,
            select_mask
        );

        cv::Mat red_mask_1;

        cv::inRange(
            hsv,
            m_lower_red_1,
            m_upper_red_1,
            red_mask_1
        );

        cv::bitwise_or(
            select_mask,
            red_mask_1,
            select_mask
        );
    }
    else if (select == "blue")
    {
        cv::inRange(
            hsv,
            m_lower_blue,
            m_upper_blue,
            select_mask
        );
    }
    cv::Mat kernel = cv::getStructuringElement(
        cv::MORPH_RECT,
        cv::Size(3, 3)
    );
    cv::morphologyEx(
        select_mask,
        select_mask,
        cv::MORPH_OPEN,
        kernel,
        cv::Point(-1, -1),
        1
    );
}