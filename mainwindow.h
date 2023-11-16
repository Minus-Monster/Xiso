#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>
#include "GraphicsWidget.h"
#include "Modules/DebugConsole.h"

#include "Detector.h"
#include "CGrabber.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
void setDebugMessage(QtMsgType type, const QMessageLogContext &conText, const QString &msg);
class MainWindow;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setGrabber(CGrabber *c);
    void setDetector(Detector *d);
    void setMessage(QString message);

signals:
    void grabbingStart(int numFrame=0);
    void grabbingFinished();

    void grabberInit();
    void dtectorInit();

private:
    Ui::MainWindow *ui;
    Qylon::GraphicsWidget *widget;
    Qylon::DebugConsole *console;
    QVBoxLayout *layout;
    CGrabber *grabber;
    Detector *detector;
    QElapsedTimer *timer;
    QString darkCalPath="";
    QString brightCalPath="";
    uint8_t* cal_Data0 = NULL;
    uint8_t* cal_Data1 = NULL;
    uint8_t* cal_Data2 = NULL;
    uint8_t* cal_Data3 = NULL;

};
#endif // MAINWINDOW_H
