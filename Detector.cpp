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

    /*
    memcpy(image.GetDataPointer(0), pImg, sizeof(SpectrumLogic::ushort) * *pWidth * *pHeight);

    if (!image.WriteTiffImage("C:/GoLiveCallbackCapture" + std::to_string(*pFrameCount + 1) + ".tif", image, 16))
        qDebug() << "[ERROR]: Failed to save frame #" << *pFrameCount + 1;

    */
    ++*pFrameCount;
    emit instance->sendBuffer(pImg);
}

void Detector::sequentialGrabbing(int frameCount) {
    qDebug() << "Exp :" << exposureTime << ", FrameCount : " << frameCount << "\n";

    sl_device.GoLive(callBackLive, &frameCount);
}

void Detector::stopGrabLive() {
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

void Detector::setExposureTime(int us){
    sl_device.SetExposureTime(exposureTime);
    exposureTime = us;
}
