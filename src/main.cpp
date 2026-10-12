#include <opencv2/opencv.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "video_capture.h"
#include "color_detector.h"
#include "light_bar.h"
#include "armor.h"

struct ArmorPair
{
    int first;
    int second;
    double score;
};

double get_long_axis_angle(const cv::RotatedRect& rect)
{
    cv::Point2f points[4];
    rect.points(points);

    double max_length = 0.0;
    double angle = 0.0;

    for (int i = 0; i < 4; ++i)
    {
        cv::Point2f edge = points[(i + 1) % 4] - points[i];
        double length = std::sqrt(edge.x * edge.x + edge.y * edge.y);

        if (length > max_length)
        {
            max_length = length;
            angle = std::atan2(edge.y, edge.x) * 180.0 / CV_PI;
        }
    }

    while (angle >= 90.0)
    {
        angle -= 180.0;
    }

    while (angle < -90.0)
    {
        angle += 180.0;
    }

    return angle;
}

double get_angle_difference(double angle1, double angle2)
{
    double difference = std::abs(angle1 - angle2);

    if (difference > 90.0)
    {
        difference = 180.0 - difference;
    }

    return difference;
}

bool is_valid_light_bar(
    const std::vector<cv::Point>& contour,
    double frame_area,
    LightBar& light_bar)
{
    double area = cv::contourArea(contour);

    if (area < 10.0 || area > frame_area * 0.05)
    {
        return false;
    }

    cv::RotatedRect rect = cv::minAreaRect(contour);

    double length = std::max(rect.size.width, rect.size.height);
    double width = std::min(rect.size.width, rect.size.height);

    if (length < 8.0 || width < 1.0)
    {
        return false;
    }

    double aspect_ratio = length / width;

    if (aspect_ratio < 2.0 || aspect_ratio > 10.0)
    {
        return false;
    }

    double rect_area = length * width;

    if (rect_area <= 0.0 || area / rect_area < 0.2)
    {
        return false;
    }

    double angle = get_long_axis_angle(rect);
    double vertical_deviation = 90.0 - std::abs(angle);

    if (vertical_deviation > 20.0)
    {
        return false;
    }

    light_bar.rect = rect;
    light_bar.length = length;
    light_bar.width = width;
    light_bar.angle = angle;

    return true;
}

bool is_valid_pair(
    const LightBar& left,
    const LightBar& right,
    double& score)
{
    cv::Point2f p1 = left.rect.center;
    cv::Point2f p2 = right.rect.center;

    if (p1.x > p2.x)
    {
        std::swap(p1, p2);
    }

    double average_length = (left.length + right.length) / 2.0;

    if (average_length <= 0.0)
    {
        return false;
    }

    double length_ratio =
        std::min(left.length, right.length) /
        std::max(left.length, right.length);

    if (length_ratio < 0.35)
    {
        return false;
    }

    double angle_diff = get_angle_difference(left.angle, right.angle);

    if (angle_diff > 25.0)
    {
        return false;
    }

    double vertical_diff = std::abs(p1.y - p2.y);

    if (vertical_diff > average_length * 0.8)
    {
        return false;
    }

    double horizontal_distance = std::abs(p2.x - p1.x);
    double distance_ratio = horizontal_distance / average_length;

    if (distance_ratio < 0.6 || distance_ratio > 5.0)
    {
        return false;
    }

    double width_ratio =
        std::min(left.width, right.width) /
        std::max(left.width, right.width);

    if (width_ratio < 0.35)
    {
        return false;
    }

    score = angle_diff * 2.0
          + vertical_diff / average_length * 20.0
          + (1.0 - length_ratio) * 30.0
          + std::abs(distance_ratio - 1.8) * 5.0;

    return true;
}

void draw_light_bar(cv::Mat& frame, const LightBar& light_bar)
{
    cv::Point2f points[4];
    light_bar.rect.points(points);

    for (int i = 0; i < 4; ++i)
    {
        cv::line(
            frame,
            points[i],
            points[(i + 1) % 4],
            cv::Scalar(0, 255, 0),
            2);
    }
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
    std::string select = "blue";

    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::namedWindow("Selected Mask", cv::WINDOW_NORMAL);

    cv::resizeWindow("Original", 640, 480);
    cv::resizeWindow("Selected Mask", 640, 480);

    cv::moveWindow("Original", 0, 0);
    cv::moveWindow("Selected Mask", 800, 0);

    cv::Mat frame;
    cv::Mat select_mask;

    while (reader.read_frame(frame))
    {
        detector.detect(frame, select, select_mask);

        std::vector<std::vector<cv::Point>> contours;

        cv::findContours(
            select_mask.clone(),
            contours,
            cv::RETR_EXTERNAL,
            cv::CHAIN_APPROX_SIMPLE);

        double frame_area =
            static_cast<double>(frame.cols) * frame.rows;

        std::vector<LightBar> light_bars;

        for (const auto& contour : contours)
        {
            LightBar light_bar;

            if (is_valid_light_bar(contour, frame_area, light_bar))
            {
                light_bars.push_back(light_bar);
            }
        }

        std::vector<ArmorPair> candidates;

        for (int i = 0; i < static_cast<int>(light_bars.size()); ++i)
        {
            for (int j = i + 1; j < static_cast<int>(light_bars.size()); ++j)
            {
                double score = 0.0;

                if (is_valid_pair(light_bars[i], light_bars[j], score))
                {
                    candidates.push_back({i, j, score});
                }
            }
        }

        std::sort(
            candidates.begin(),
            candidates.end(),
            [](const ArmorPair& a, const ArmorPair& b)
            {
                return a.score < b.score;
            });

        std::vector<bool> used(light_bars.size(), false);
        std::vector<Armor> armors;

        for (const auto& candidate : candidates)
        {
            if (used[candidate.first] || used[candidate.second])
            {
                continue;
            }

            Armor armor;
            armor.left = light_bars[candidate.first];
            armor.right = light_bars[candidate.second];

            if (armor.left.rect.center.x > armor.right.rect.center.x)
            {
                std::swap(armor.left, armor.right);
            }

            armors.push_back(armor);
            used[candidate.first] = true;
            used[candidate.second] = true;
        }

        for (const auto& armor : armors)
        {
            draw_light_bar(frame, armor.left);
            draw_light_bar(frame, armor.right);

            cv::Point2f left_center = armor.left.rect.center;
            cv::Point2f right_center = armor.right.rect.center;

            cv::line(
                frame,
                left_center,
                right_center,
                cv::Scalar(0, 0, 255),
                2);

            cv::Point2f center(
                (left_center.x + right_center.x) / 2.0f,
                (left_center.y + right_center.y) / 2.0f);

            cv::circle(
                frame,
                center,
                3,
                cv::Scalar(255, 0, 255),
                -1);
        }

        cv::imshow("Original", frame);
        cv::imshow("Selected Mask", select_mask);

        char key = static_cast<char>(cv::waitKey(1));

        if (key == 27)
        {
            break;
        }

        if (key == 'r' || key == 'R')
        {
            select = "red";
        }
        else if (key == 'b' || key == 'B')
        {
            select = "blue";
        }
    }

    cv::destroyAllWindows();

    return 0;
}