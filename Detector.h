#ifndef DETECTOR_H
#define DETECTOR_H

#include "SLDevice.h"
#include "SLImage.h"
#include <iostream>
#include <QObject>
#include <QDebug>

class Detector : public QObject
{
    Q_OBJECT
public:
    Detector() {}
    ~Detector(){}
    bool initialize();
    static void callBackLive(SpectrumLogic::ushort* pImg, int* pWidth, int* pHeight, SpectrumLogic::SLError* err, void* userArgs);
    void sequentialGrabbing(int frameCount);
    void stopGrabLive();
    void setROI(int _w, int _h, int _x, int _y);
    void setWidth(int _w);
    int getWidth();
    void setHeight(int _h);
    int getHeight();
    void setX(int _x);
    int getX();
    void setY(int _y);
    int getY();
    void setExposureTime(int us);
    int getExposureTime(){ return exposureTime; }

signals:
    void transfer();
    void sendBuffer(const void* buffer);

private:
    SpectrumLogic::SLDevice sl_device;
    int exposureTime = 3000;
    SpectrumLogic::DeviceInterface sl_iFace{ SpectrumLogic::S2I_GIGE };
};
#endif // DETECTOR_H
