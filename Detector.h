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
    void continuousGrabbing();
    void stopGrabbing();
    bool setROI(int _w, int _h, int _x, int _y);
    void setWidth(int _w);
    int getWidth();
    void setHeight(int _h);
    int getHeight();
    void setX(int _x);
    int getX();
    void setY(int _y);
    int getY();
    bool setExposureTime(int us);
    int getExposureTime(){ return exposureTime; }
    bool setExposureMode(SpectrumLogic::ExposureModes mode);
    SpectrumLogic::ExposureModes getExposureMode();
    bool setBinningMode(SpectrumLogic::BinningModes mode);
    SpectrumLogic::BinningModes getBinningMode();
    void setSaveMode(bool on){ saveMode = on; }
    void setSavingPath(QString path){ savePath = path; }
    QString getSavingPath(){ return savePath; }
    bool isSaveMode(){ return saveMode; }

signals:
    void sendBuffer(const unsigned short* buffer);

private:
    SpectrumLogic::SLDevice sl_device;
    int exposureTime = 50;
    SpectrumLogic::DeviceInterface sl_iFace{ SpectrumLogic::S2I_GIGE };
    bool saveMode = false;
    QString savePath ="";

};
#endif // DETECTOR_H
