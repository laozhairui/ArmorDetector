#pragma once

#include <opencv2/opencv.hpp>
#include <string>

class ColorDetecter
{
public:
    void detect(
        const cv::Mat& frame,
        const std::string& select,
        cv::Mat& select_mask
    );

private:
    cv::Scalar m_lower_red{0, 80, 50};
    cv::Scalar m_upper_red{10, 255, 255};

    cv::Scalar m_lower_red_1{160, 80, 50};
    cv::Scalar m_upper_red_1{179, 255, 255};

    cv::Scalar m_lower_blue{90, 80, 50};
    cv::Scalar m_upper_blue{135, 255, 255};
};