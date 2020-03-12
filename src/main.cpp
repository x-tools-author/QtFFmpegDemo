#include <QDebug>
#include "Application.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    Application app(argc, argv);

    app.setupUi();

    return app.exec();
}
