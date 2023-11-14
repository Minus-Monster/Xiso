#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Acquisition/Camera.h"
#include "Modules/GraphicsWidget.h"
#include "Detector.h"
#include "CGrabber.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setGrabber(CGrabber *c);
    void setDetector(Detector *d);
    void setCamera(Qylon::Camera *c);

private:
    Ui::MainWindow *ui;
    Qylon::GraphicsWidget *widget;
    Qylon::Camera *camera;
    QVBoxLayout *layout;
    CGrabber *grabber;
    Detector *detector;
};
#endif // MAINWINDOW_H
