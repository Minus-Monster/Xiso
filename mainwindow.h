#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Detector.h"
#include "Acquisition/Grabber.h"
#include "Modules/GraphicsWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setGrabber(Qylon::Grabber *g);
    void setDetector(Detector *d);

private:
    Ui::MainWindow *ui;
    Qylon::GraphicsWidget *widget;
    Qylon::Grabber *grabber;
    QVBoxLayout *layout;

};
#endif // MAINWINDOW_H
