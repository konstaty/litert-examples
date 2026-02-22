/*
SPDX-License-Identifier: BSD-3-Clause

Copyright (c) 2026 Konstantin Tyurin <konstantin@pluraf.com>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <algorithm>
#include <fstream>
#include <cmath>

#include <opencv2/opencv.hpp>

#include "litert/cc/litert_common.h"
#include "litert/cc/litert_compiled_model.h"
#include "litert/cc/litert_environment.h"
#include "litert/cc/litert_expected.h"
#include "litert/cc/litert_macros.h"
#include "litert/cc/litert_options.h"
#include "litert/cc/litert_tensor_buffer.h"
#include "litert/cc/options/litert_cpu_options.h"
#include "litert/cc/options/litert_gpu_options.h"
#include "litert/cc/options/litert_qualcomm_options.h"


using namespace litert;
using namespace std;


litert::HwAcceleratorSet GetAccelerator() {
    litert::HwAcceleratorSet accelerators(litert::HwAccelerators::kNone);
    accelerators |= litert::HwAccelerators::kCpu;
    return accelerators;
}


std::vector<std::string> read_labels(std::string const &file_name)
{
    std::vector<string> labels;

    std::ifstream file(file_name);
    if( !file )
    {
        throw std::runtime_error("Failed to load labels");
    }

    std::string line;
    while (std::getline(file, line)) { labels.push_back(line); }

    return labels;
}


void draw_boxes(
    cv::Mat &image,
    double input_width,
    double input_height,
    bool ratio_preserved,
    std::vector<float> &classes,
    std::vector<float> &probabilities,
    std::vector<float> &boxes)
{
    static auto labels = read_labels("labelmap.txt");

    double scale_x = 1;
    double scale_y = 1;

    if( ratio_preserved )
    {
        double orig_ratio = image.cols / image.rows;
        double input_ratio = input_width / input_height;

        if( input_ratio <= orig_ratio )
        {
            scale_x = image.cols / input_width;
            scale_y = scale_x;
        }
        else
        {
            scale_y = image.rows / input_height;
            scale_x = scale_y;
        }
    }
    else
    {
        scale_x = image.cols / input_width;
        scale_y = image.rows / input_height;
    }

    cout << scale_x << " " << scale_y << endl;

    for( int i = 0; i < 100; ++i )
    {
        if( probabilities[i] < 0.4 ){ continue; }

        auto pos = i * 4;
        int left = std::clamp<int>(boxes[pos + 1] * input_width, 0, input_width) * scale_x;
        int top = std::clamp<int>(boxes[pos + 0] * input_height, 0, input_height)  * scale_x; //
        int right = std::clamp<int>(boxes[pos + 3] * input_width, 0, input_width) * scale_x;
        int bottom = std::clamp<int>(boxes[pos + 2] * input_height, 0, input_height) * scale_x; //

        cv::rectangle(image,
                  cv::Point(left, top),
                  cv::Point(right, bottom),
                  0.5, 1);

        string text = labels[classes[i]] + ": " + to_string(static_cast<int>(probabilities[i] * 100)) + "%";
        int font = cv::FONT_HERSHEY_DUPLEX;
        double fontHeight = 0.4 * std::min(scale_y, 4.);
        int baseline = 0;
        cv::Size text_size = cv::getTextSize(text, font, fontHeight, 3, &baseline);

        cv::rectangle(image,
                    cv::Point(left, top - text_size.height - baseline - 2),
                    cv::Point(left + text_size.width, top),
                    cv::Scalar(0,0,0));

        cv::putText(image, text,
                cv::Point(left, top - baseline),
                font, fontHeight, 3);
    }
}


int main(int argc, char *argv[])
{
    LITERT_ASSIGN_OR_RETURN(auto env, Environment::Create({}));

    auto accelerator = GetAccelerator();
    std::vector<litert::Environment::Option> environment_options = {};
    LITERT_ASSIGN_OR_RETURN(auto options, Options::Create());

    if (accelerator & litert::HwAccelerators::kCpu) {
        LITERT_ASSIGN_OR_RETURN(auto& cpu_compilation_options, options.GetCpuOptions());
        LITERT_RETURN_IF_ERROR(cpu_compilation_options.SetNumThreads(4));
        options.SetHardwareAccelerators(accelerator);
    }

    LITERT_ASSIGN_OR_RETURN(auto compiled_model,
        CompiledModel::Create(env, "ssdlite_mobiledet_cpu_320x320_coco_2020_05_19.tflite", options));

    LITERT_ASSIGN_OR_RETURN(auto input_buffers, compiled_model.CreateInputBuffers());
    cout << "~~~ INPUT ~~~" << endl;
    cout << input_buffers.size() << endl;
    for (int i = 0; i < input_buffers.size(); ++i)
    {
        auto s = input_buffers[i].Size();
        cout << "tensor " << i << " size " << s.Value() << endl;
    }
    cout << "~~~ OUTPUT ~~~" << endl;
    LITERT_ASSIGN_OR_RETURN(auto output_buffers, compiled_model.CreateOutputBuffers());
    cout << output_buffers.size() << endl;
    for (int i = 0; i < output_buffers.size(); ++i)
    {
        auto s = output_buffers[i].Size();
        cout << "tensor " << i << " size " << s.Value() << endl;
    }

    string image_path {"2-1.jpg"};
    cv::Mat image = cv::imread(image_path);

    if (image.empty()) {
        std::cerr << "Failed to load image: " << image_path << std::endl;
        return -1;
    }

    bool preserve_ratio = true;

    double orig_h = image.rows;
    double orig_w = image.cols;
    double orig_ratio = orig_w / orig_h;

    double input_width = 320;
    double input_height = 320; //(320. / orig_w) * orig_h;
    double input_ratio = input_width / input_height;

    cv::Mat resized;

    if( fabs(orig_ratio - input_ratio) > 0.1 && preserve_ratio )
    {
        int resized_width;
        int resized_height;

        if( input_ratio <= orig_ratio )
        {
            resized_width = input_width;
            resized_height = input_width / orig_ratio;
        }
        else
        {
            resized_height = input_height;
            resized_width = input_height * orig_ratio;
        }
        resized = cv::Mat(input_width, input_height, CV_8UC3, cv::Scalar(190, 190, 190));
        cv::Mat tmp_img;
        cv::resize(image, tmp_img, cv::Size(resized_width, resized_height));
        cv::Mat roi = resized(cv::Rect(0, 0, tmp_img.cols, tmp_img.rows));
        tmp_img.copyTo(roi);
    }
    else
    {
        cv::resize(image, resized, cv::Size(input_width, input_height));
    }
    cv::cvtColor(resized, resized, cv::COLOR_BGR2RGB);


    cout << input_width << " x " << input_height << endl;

    float input_data[320 * 320 * 3];

    for (int y = 0; y < input_height; ++y) {
        for (int x = 0; x < input_width; ++x) {
            cv::Vec3b pixel = resized.at<cv::Vec3b>(y, x);
            int idx = (y * input_width + x) * 3;
            input_data[idx + 0] = static_cast<float>(pixel[0]) / 255.;  // R
            input_data[idx + 1] = static_cast<float>(pixel[1]) / 255.;  // G
            input_data[idx + 2] = static_cast<float>(pixel[2]) / 255.;  // B
        }
    }

    input_buffers[0].Write<float>(absl::MakeConstSpan(input_data, sizeof(input_data) / 4));

    compiled_model.Run(input_buffers, output_buffers);

    std::vector<float> data(1);
    output_buffers[3].Read<float>(absl::MakeSpan(data));
    cout << data[0] << endl;

    std::vector<float> classes(100);
    output_buffers[1].Read<float>(absl::MakeSpan(classes));
    for (int i = 0; i < 100; ++i)
    {
        cout << classes[i] << endl;
    }

    std::vector<float> probabilities(100);
    output_buffers[2].Read<float>(absl::MakeSpan(probabilities));
    for (int i = 0; i < 100; ++i)
    {
        cout << probabilities[i] << endl;
    }

    std::vector<float> boxes(400);
    output_buffers[0].Read<float>(absl::MakeSpan(boxes));

    draw_boxes(resized, 320, 320, preserve_ratio, classes, probabilities, boxes);
    draw_boxes(image, 320, 320, preserve_ratio, classes, probabilities, boxes);

    cv::imwrite("resized.jpg", resized);
    cv::imwrite("original.jpg", image);

    std::cout << "DONE\n";

    return 0;
}