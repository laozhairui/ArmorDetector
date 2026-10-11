#pragma once

#include <opencv2/opencv.hpp>

struct LightBar
{
    cv::RotatedRect rect;

    double length;

    double width;

    double angle;
};