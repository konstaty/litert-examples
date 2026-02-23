#ifndef __LITERT_EXAMPLES_CORE__
#define __LITERT_EXAMPLES_CORE__


#include <string>
#include <vector>
#include <filesystem>

#include <opencv2/opencv.hpp>


int process(
    std::string const &model_path,
    std::string const &image_path,
    std::vector<cv::Mat> &out
);


class Model
{
    std::vector<std::string> models_;
    std::vector<std::string> images_;
    uint32_t img_ix_{ 0 };
public:
    Model()
    {
        namespace fs = std::filesystem;

        for( auto const &entry : fs::directory_iterator(".") )
        {
            if( entry.is_regular_file() && entry.path().extension().string() == ".tflite" )
            {
                models_.push_back(entry.path().filename().string());
            }
        }

        for( auto const &entry : fs::directory_iterator("./images") )
        {
            if( entry.is_regular_file() && entry.path().extension().string() == ".jpg" )
            {
                images_.push_back(entry.path().string());
            }
        }
    }

    std::vector<std::string> const &models()
    {
        return models_;
    }

    std::string const &image()
    {
        if( img_ix_ >= images_.size() ){ img_ix_ = 0; }

        return images_[img_ix_];
    }

    std::string const &next_image()
    {
        if( img_ix_ >= images_.size() ){ img_ix_ = 0; }

        auto const &image_path = images_[img_ix_++];
        return image_path;
    }


    std::vector<cv::Mat> process(std::string const model_path, std::string const image_path)
    {
        std::vector<cv::Mat> images;

        ::process(model_path,
            image_path,
            images
        );

        return images;
    }
};


#endif  // __LITERT_EXAMPLES_CORE__