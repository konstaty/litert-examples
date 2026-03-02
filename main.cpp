#include "gui/window.hpp"
#include "core/core.hpp"


using namespace std;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Model model;

    MainWindow window{ model };
    window.show();

    return QApplication::exec();
}
