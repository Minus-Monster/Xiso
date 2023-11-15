#include "mainwindow.h"
#include "Qylon.h"
#include "Detector.h"
#include "CGrabber.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;


    CGrabber *g = new CGrabber;
    w.setGrabber(g);
    QObject::connect(&w, &MainWindow::grabberInit, g, [g](){
        auto applet = QFileDialog::getOpenFileName(nullptr, "Load an applet", QDir::homePath(), "*.hap *.dll");
        if(applet.isEmpty()) return;

        auto config = QFileDialog::getOpenFileName(nullptr, "Load a configuration file", QDir::homePath(), "*.mcf");
        if(config.isEmpty()) return;

        g->loadApplet(applet);
        g->loadConfiguration(config);
//        g->loadApplet("C:/Program Files/Basler/FramegrabberSDK/Hardware Applets/IMP-CX-4S/Innometry_10G_Project.hap");
//        g->loadConfiguration("C:/Users/User/Desktop/10G/SimpleTest/Inno.mcf");
        g->initialize();
        QMessageBox::information(nullptr, "Basler Framegrabber", "Loading is finished.");
    });


    Detector *d = new Detector;
    w.setDetector(d);
    QObject::connect(d, &Detector::sendBuffer, g, &CGrabber::convertToGrabberImage);
    QObject::connect(&w, &MainWindow::grabbingStart, d, [d, g](int num){
        if(num==0){ // Continuous Grabbing
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


//    Qylon::Qylon q;
//    auto camera = q.addCamera();
//    camera->openCamera("Basler acA2440-20gc (23131022)");
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



    w.show();
    return a.exec();
}
