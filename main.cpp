#include "gui/window.hpp"
#include "core/core.hpp"


using namespace std;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // "ssdlite_mobiledet_cpu_320x320_coco_2020_05_19.tflite"
    // "coco_ssd_mobilenet_v1_1.0_quant_2018_06_29.tflite"

    Model model;

    MainWindow window{ model };
    window.show();

    return QApplication::exec();
}
