//#include "Qylon.h"
#include "mainwindow.h"
#include "Detector.h"
#include "Grabber.h"
#include <QApplication>
#include <QFileDialog>
#include <QTime>

MainWindow* window = nullptr;
void setDebugMessage(QtMsgType type, const QMessageLogContext &conText, const QString &msg)
{
    window->setMessage("[" + QTime::currentTime().toString() + "] " + msg);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    window = &w;
    qInstallMessageHandler(setDebugMessage);

    Grabber *g = new Grabber;
    w.setGrabber(g);

    Detector *d = new Detector;
    w.setDetector(d);
    // Need to send the grabber
    QObject::connect(&w, &MainWindow::grabbingStart, d, [d, g](int num){
        if(num==0){
            g->continuousGrabbing();
            d->continuousGrabbing();
        }else{
            g->sequentialGrabbing(num);
            d->sequentialGrabbing(num+1);
        }
    });
    QObject::connect(&w, &MainWindow::grabbingFinished, d, [d, g](){
        g->stopGrabbing();
        d->stopGrabbing();
    });
    w.showMaximized();

    return a.exec();
}
