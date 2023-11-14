#include "mainwindow.h"
#include "Qylon.h"
#include "Detector.h"
#include "CGrabber.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    Qylon::Qylon q;
    w.setCamera(q.addCamera());

    CGrabber *g = new CGrabber;
    w.setGrabber(g);

//    Detector *d = new Detector;
//    w.setDetector(d);

    w.show();

    return a.exec();
}
