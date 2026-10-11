#include <video_capture.h>


VideoCapture::VideoCapture(const std::string& video_path)
{
    m_capture.open(video_path);
}


bool VideoCapture::is_opened() const
{
    return m_capture.isOpened();
}


bool VideoCapture::read_frame(cv::Mat& frame)
{
    return m_capture.read(frame);
}
