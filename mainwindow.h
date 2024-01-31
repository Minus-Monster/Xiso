#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>
#include <QTextEdit>
#include "GraphicsWidget.h"
//#include "Modules/DebugConsole.h"

#include "Detector.h"
#include "Grabber.h"
#include "BrightContrastControl.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
void setDebugMessage(QtMsgType, const QMessageLogContext &, const QString &msg);
class MainWindow;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setGrabber(Grabber *c);
    void setDetector(Detector *d);
    void setMessage(QString message);
    void setROI();
    void histogramEqualization(QImage &image);
    bool averagedImage(QString avgFileName, QString dirPath, QStringList fileNameList);
    bool generateLUT(QString darkAvgFileName, QString brightAvgFileName, QString savePath);

signals:
    void grabbingStart(int numFrame=0);
    void grabbingFinished();

    void grabberInit();
    void dtectorInit();

private:
    Ui::MainWindow *ui;
    Qylon::GraphicsWidget *widget;
    QVBoxLayout *layout;
    Grabber *grabber;
    Detector *detector;
    QElapsedTimer *timer;
    QString darkCalPath="";
    QString brightCalPath="";

    QDockWidget *dockwidgetDebug;
    QTextEdit *debugConsole;

    QDockWidget *dockwidgetDetector;

    QDockWidget *dockwidgetGrabber;


    QMetaObject::Connection grabberConnect;
    QMetaObject::Connection detectorConnect;
    BrightContrastControl *bcControl;

};
#endif // MAINWINDOW_H
