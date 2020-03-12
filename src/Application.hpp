#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QGuiApplication>
#include <QQmlApplicationEngine>

class Decoder;
class Application : public QGuiApplication
{
    Q_OBJECT
public:
    explicit Application(int argc, char **argv);
    ~Application();

    void setupUi();
private:
    QQmlApplicationEngine engine;
    Decoder *decoder;
signals:

};

#endif // APPLICATION_HPP
