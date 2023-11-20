#include "Detector.h"
#include "DetectorDialog.h"
Detector *instance = nullptr;

int frameCount = 0;
void callBackLive(SpectrumLogic::ushort *pImg, int *pWidth, int *pHeight, SpectrumLogic::SLError *err, void *userArgs) {
    int* pFrameCount = (int*)userArgs;
    if (*err != SpectrumLogic::SLError::SL_ERROR_SUCCESS)
    {
        qDebug() << "[ERROR]: Failed to get frame" ;
        return;
    }

    //    free(instance->currentBuffer);
    instance->currentBuffer = (unsigned short*)malloc((size_t)(*pWidth * *pHeight * 2));
    memcpy(instance->currentBuffer, pImg, sizeof(SpectrumLogic::ushort) * *pWidth * *pHeight);

    SpectrumLogic::SLImage image;
    image.Build(*pWidth, *pHeight, 1);
    if(instance->isSaveMode()){
        qDebug() << "entranced to saving mode";
        memcpy(image.GetDataPointer(0), pImg, sizeof(SpectrumLogic::ushort) * *pWidth * *pHeight);
        if (!image.WriteTiffImage(instance->getSavingPath().toStdString() + std::to_string(*pFrameCount) + ".tiff", image, 16)){
            qDebug() << "[ERROR]: Failed to save frame #" << *pFrameCount + 1;
        }
    }
    qDebug() << "For checking" << *pFrameCount << frameCount;
    ++*pFrameCount;
    emit instance->sendBuffer(instance->currentBuffer);

}

Detector::Detector() {
    dialog = new DetectorDialog;
    dialog->setDetector(this);
}

bool Detector::initialize(){
    qDebug() << "initializing SpectrumLogic";
    sl_device = SpectrumLogic::SLDevice(sl_iFace);

    if (sl_device.OpenCamera() != SpectrumLogic::SLError::SL_ERROR_SUCCESS) {
        qDebug() << "Failed to open a SL camera.";
        return false;
    }
    else qDebug() << "Opened a SL camera.";

    qDebug()  << "Setting Full Well low" << SpectrumLogic::SLErrorToString(sl_device.SetFullWell(SpectrumLogic::FullWellModes::Low)).c_str();
    qDebug()<< "Setting test mode false" << SpectrumLogic::SLErrorToString(sl_device.SetTestMode(false)).c_str() ;
    qDebug() << "Setting Binning mode X11"<< SpectrumLogic::SLErrorToString(sl_device.SetBinningMode(SpectrumLogic::BinningModes::x11)).c_str() ;
    qDebug() << "Setting Exposure mode Seq"<< SpectrumLogic::SLErrorToString(sl_device.SetExposureMode(SpectrumLogic::ExposureModes::seq_mode)).c_str() ;
    qDebug() << "Setting Expousre time 12"<< SpectrumLogic::SLErrorToString(sl_device.SetExposureTime(12)).c_str() ;

    instance = this;
    return true;
}


void Detector::sequentialGrabbing(int numFrame) {
    if(!sl_device.IsConnected()){
        qDebug() << "SL Device is not connected. Nothing will be changed.";
        return;
    }
    qDebug() << setExposureMode(SpectrumLogic::ExposureModes::seq_mode);

    sl_device.SetNumberOfFrames(numFrame);

    frameCount = 0;
    sl_device.GoLive(callBackLive, &frameCount);
    sl_device.SoftwareTrigger();
}

void Detector::continuousGrabbing()
{
    frameCount = 0;
    sl_device.GoLive(callBackLive, &frameCount);
}

void Detector::stopGrabbing() {
    if(!sl_device.IsConnected()){
        qDebug() << "SL Device is not connected. Nothing will be changed.";
        return;
    }
    sl_device.GoUnLive();
}

bool Detector::setROI(int _w, int _h, int _x, int _y)
{
    if(!sl_device.IsConnected()){
        qDebug() << "Detector is not connected with current system.";
        return false;
    }
    SpectrumLogic::ROIinfo roi;
    roi.W = _w;
    roi.H = _h;
    roi.X = _x;
    roi.Y = _y;
    auto error = sl_device.SetROI(roi);
    qDebug() << SpectrumLogic::SLErrorToString(error).c_str();

    if(error == SpectrumLogic::SLError::SL_ERROR_ACCESS) return true;
    else return false;
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

bool Detector::setExposureTime(int ms){
    if(!sl_device.IsConnected()){
        qDebug() << "SL Device is not connected. Nothing will be changed.";
        return false;
    }
    auto error = sl_device.SetExposureTime(exposureTime);
    qDebug() << "Set exposure time : " << SpectrumLogic::SLErrorToString(error).c_str();
    if(error != SpectrumLogic::SLError::SL_ERROR_SUCCESS) return false;

    exposureTime = ms;

    return true;
}

int Detector::getExposureTime(){
    int integrationTimeHigh = sl_device.RegisterRead(AddrExposureTimeHigh);
    int integrationTimeLow = sl_device.RegisterRead(AddrExposureTimeLow);
    int readoutTimeHight = max(sl_device.RegisterRead(AddrReadOutTimeHigh),0);
    int readoutTimeLow = max(sl_device.RegisterRead(AddrReadOutTimeLow),0);
    int readoutTime_10us = ((readoutTimeHight << 16) | readoutTimeLow) / 10;
    int integrationTime_10us = ((integrationTimeHigh << 16) | integrationTimeLow);
    exposureTime = (readoutTime_10us + integrationTime_10us) / 100;
    return exposureTime;
}

bool Detector::setExposureMode(SpectrumLogic::ExposureModes mode)
{
    auto error = sl_device.SetExposureMode(mode);
    qDebug() << "Set exposure mode :" << SpectrumLogic::SLErrorToString(error).c_str();
    if(error != SpectrumLogic::SLError::SL_ERROR_SUCCESS) return false;
    else return true;
}

SpectrumLogic::ExposureModes Detector::getExposureMode()
{
    SpectrumLogic::ExposureModes mode;
    qDebug() << "Get exposure mode : " << SpectrumLogic::SLErrorToString(sl_device.GetExposureMode(mode)).c_str();
    return mode;
}

bool Detector::setBinningMode(SpectrumLogic::BinningModes mode){
    auto error = sl_device.SetBinningMode(mode);
    qDebug()<< "Set binning mode : " <<SpectrumLogic::SLErrorToString(error).c_str();
    if(error != SpectrumLogic::SLError::SL_ERROR_SUCCESS){
        return false;
    }else return true;
}

SpectrumLogic::BinningModes Detector::getBinningMode(){
    return sl_device.GetBinningMode();
}

bool Detector::setFullWell(SpectrumLogic::FullWellModes mode)
{
    auto error = sl_device.SetFullWell(mode);
    qDebug() << "Set full well : " << SpectrumLogic::SLErrorToString(sl_device.SetFullWell(mode)).c_str();
    if(error != SpectrumLogic::SLError::SL_ERROR_SUCCESS){
        return false;
    }else return true;
}

SpectrumLogic::FullWellModes Detector::getFullWellMode()
{
    return SpectrumLogic::FullWellModes::Low;
}
