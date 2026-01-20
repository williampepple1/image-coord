#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Image Map Generator");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ImageCoord");

    MainWindow window;
    window.show();

    return app.exec();
}
