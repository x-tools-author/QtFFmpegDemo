#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QGuiApplication>
#include <QQmlApplicationEngine>

class Decoder;
class ImageProvider;
class Application : public QGuiApplication
{
    Q_OBJECT
public:
    explicit Application(int argc, char **argv);
    ~Application();

    void setupUi();
private:
    QQmlApplicationEngine engine;
    ImageProvider *imageProvider;
    Decoder *decoder;
};

#endif // APPLICATION_HPP
