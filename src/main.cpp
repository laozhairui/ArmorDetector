#include <video_capture.h>

#include <iostream>


int main()
{
    VideoCapture reader("data/test.mp4");

    if (!reader.is_opened())
    {
        std::cerr << "Failed to open video." << std::endl;

        return -1;
    }

    cv::Mat frame;

    while (reader.read_frame(frame))
    {
        cv::imshow("Video", frame);

        if (cv::waitKey(1) == 27)
        {
            break;
        }
    }

    return 0;
}
