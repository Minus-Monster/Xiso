#ifndef DETECTOR_H
#define DETECTOR_H

#include "DetectorDialog.h"
#include "SLDevice.h"
#include "SLImage.h"
#include <iostream>
#include <QObject>
#include <QDebug>
#include <QDialog>

class DetectorDialog;
class Detector : public QObject
{
    Q_OBJECT
public:
    Detector();
    ~Detector(){}
    bool initialize();
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
    bool setExposureTime(int ms);
    int getExposureTime();
    bool setExposureMode(SpectrumLogic::ExposureModes mode);
    SpectrumLogic::ExposureModes getExposureMode();

    bool setBinningMode(SpectrumLogic::BinningModes mode);
    SpectrumLogic::BinningModes getBinningMode();

    bool setFullWell(SpectrumLogic::FullWellModes mode);
    SpectrumLogic::FullWellModes getFullWellMode();

    void setSaveMode(bool on){ saveMode = on; }
    void setSavingPath(QString path){ savePath = path; }
    QString getSavingPath(){ return savePath; }
    bool isSaveMode(){ return saveMode; }
    unsigned short* currentBuffer;
    QDialog* getDialog(){ return dynamic_cast<QDialog*>(dialog);}

signals:
    void sendBuffer(unsigned short* buffer);

private:
    SpectrumLogic::SLDevice sl_device;
    int exposureTime = 0;
    SpectrumLogic::DeviceInterface sl_iFace{ SpectrumLogic::S2I_GIGE };
    bool saveMode = false;
    QString savePath ="";

    DetectorDialog *dialog;

};
#endif // DETECTOR_H
