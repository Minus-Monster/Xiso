#include "Detector.h"

Detector *instance = nullptr;
bool Detector::initialize(){
    qDebug() << "initializing SpectrumLogic";
    sl_device = SpectrumLogic::SLDevice(sl_iFace);

    if (sl_device.OpenCamera() != SpectrumLogic::SLError::SL_ERROR_SUCCESS) {
        qDebug() << "Failed to open a SL camera.";
        return false;
    }
    else qDebug() << "Opened a SL camera.";

    setBinningMode(SpectrumLogic::BinningModes::x11);
    sl_device.SetFullWell(SpectrumLogic::FullWellModes::Low);
    sl_device.SetTestMode(false);

    instance = this;
    return true;
}

void Detector::callBackLive(SpectrumLogic::ushort *pImg, int *pWidth, int *pHeight, SpectrumLogic::SLError *err, void *userArgs) {
    int* pFrameCount = (int*)userArgs;

    if (*err != SpectrumLogic::SLError::SL_ERROR_SUCCESS)
    {
        qDebug() << "[ERROR]: Failed to get frame" ;
        return;
    }
    qDebug() << "[INFO]: Caught frame #" << *pFrameCount + 1 << " with dimensions " << *pWidth << " x " << *pHeight;
    SpectrumLogic::SLImage image;
    image.Build(*pWidth, *pHeight, 1);

    if(instance->isSaveMode()){
        memcpy(image.GetDataPointer(0), pImg, sizeof(SpectrumLogic::ushort) * *pWidth * *pHeight);
        if (!image.WriteTiffImage(instance->getSavingPath().toStdString() + std::to_string(*pFrameCount) + ".tiff", image, 16)){
            qDebug() << "[ERROR]: Failed to save frame #" << *pFrameCount + 1;
        }
    }

    ++*pFrameCount;
    emit instance->sendBuffer(pImg);
}

void Detector::sequentialGrabbing(int numFrame) {
    if(!sl_device.IsConnected()){
        qDebug() << "SL Device is not connected. Nothing will be changed.";
        return;
    }
    setExposureMode(SpectrumLogic::ExposureModes::seq_mode);

    sl_device.SetNumberOfFrames(numFrame + 1);

    int frameCount = 0;
    sl_device.GoLive(callBackLive, &frameCount);
    sl_device.SoftwareTrigger();
}

void Detector::continuousGrabbing()
{
    int frameCount = 0;
    sl_device.GoLive(callBackLive, &frameCount);
}

void Detector::stopGrabbing() {
    if(!sl_device.IsConnected()){
        qDebug() << "SL Device is not connected. Nothing will be changed.";
        return;
    }
    sl_device.GoUnLive();
}

void Detector::setROI(int _w, int _h, int _x, int _y)
{
    if(!sl_device.IsConnected()){
        qDebug() << "Detector is not connected with current system.";
        return;
    }
    SpectrumLogic::ROIinfo roi;
    roi.W = _w;
    roi.H = _h;
    roi.X = _x;
    roi.Y = _y;

    sl_device.SetROI(roi);
}

void Detector::setWidth(int _w)
{
    setROI(_w, getHeight(), getX(), getY());
}

int Detector::getWidth()
{
    SpectrumLogic::ROIinfo roiInfo;
    sl_device.GetROI(roiInfo);
    return roiInfo.W;
}

void Detector::setHeight(int _h)
{
    setROI(getWidth(), _h, getX(), getY());
}

int Detector::getHeight()
{
    SpectrumLogic::ROIinfo roiInfo;
    sl_device.GetROI(roiInfo);
    return roiInfo.H;
}

void Detector::setX(int _x)
{
    setROI(getWidth(), getHeight(), _x, getY());
}

int Detector::getX()
{
    SpectrumLogic::ROIinfo roiInfo;
    sl_device.GetROI(roiInfo);
    return roiInfo.X;
}

void Detector::setY(int _y)
{
    setROI(getWidth(), getHeight(), getX(), _y);
}

int Detector::getY()
{
    SpectrumLogic::ROIinfo roiInfo;
    sl_device.GetROI(roiInfo);
    return roiInfo.Y;
}

bool Detector::setExposureTime(int us){
    if(!sl_device.IsConnected()){
        qDebug() << "SL Device is not connected. Nothing will be changed.";
        return false;
    }
    auto error = sl_device.SetExposureTime(exposureTime);
    qDebug() << SpectrumLogic::SLErrorToString(error).c_str();
    exposureTime = us;

    return true;
}

bool Detector::setExposureMode(SpectrumLogic::ExposureModes mode)
{
    auto error = sl_device.SetExposureMode(mode);
    qDebug() << SpectrumLogic::SLErrorToString(error).c_str();
    return true;
}

SpectrumLogic::ExposureModes Detector::getExposureMode()
{
    SpectrumLogic::ExposureModes mode;
    sl_device.GetExposureMode(mode);
    return mode;
}

bool Detector::setBinningMode(SpectrumLogic::BinningModes mode){
    auto error = sl_device.SetBinningMode(mode);
    qDebug()<< SpectrumLogic::SLErrorToString(error).c_str();
    return true;
}

SpectrumLogic::BinningModes Detector::getBinningMode(){
    return sl_device.GetBinningMode();
}
