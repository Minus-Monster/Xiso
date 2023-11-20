//#include "Qylon.h"
#include "mainwindow.h"
#include "Detector.h"
#include "CGrabber.h"
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

    CGrabber *g = new CGrabber;
    w.setGrabber(g);

    Detector *d = new Detector;
    w.setDetector(d);
    // Need to send the grabber
    QObject::connect(d, &Detector::sendBuffer, g, &CGrabber::convertToGrabberImage);
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
    w.show();

//    Qylon::Qylon q;
//    auto camera = q.addCamera();
//    camera->openCamera("Basler acA4112-8gm (40120987)");
//    QObject::connect(camera, &Qylon::Camera::grabbed, camera, [camera, g](){
////        qDebug() << "Grabbed from camera";
//        camera->drawLock();
//        const void* img = camera->getBuffer();
//        g->convertToGrabberImage((unsigned short*)img);
//    });
//    QObject::connect(&w, &MainWindow::grabbingStart, camera, [camera, g](int num){
//        if(num==0){
//            g->continuousGrabbing();
//            camera->continuousGrab();
//        }else{
//            g->sequentialGrabbing(num);
//            camera->sequentialGrab(num+1);
//        }
//    });
//    QObject::connect(&w, &MainWindow::grabbingFinished, camera, [camera, g](){
//        g->stopGrabbing();
//        camera->stopGrab();
//    });



    return a.exec();
}
