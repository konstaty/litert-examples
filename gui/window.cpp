#include "window.hpp"


QImage matToQImage(const cv::Mat &mat)
{
    int height = mat.rows;
    int width = mat.cols;

    if( mat.channels() == 3 )
    {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        return QImage(rgb.data, width, height, rgb.step, QImage::Format_RGB888).copy();
    }
    else if( mat.channels() == 1 )
    {
        return QImage(mat.data, width, height, mat.step, QImage::Format_Grayscale8).copy();
    }
    else if( mat.channels() == 4 )
    {
        return QImage(mat.data, width, height, mat.step, QImage::Format_RGBA8888).copy();
    }

    return QImage();
}
