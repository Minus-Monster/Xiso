#include "Detector.h"
#include "DetectorDialog.h"
#include <QImage>

Detector *instance = nullptr;

int startFrameNum =0;
int loopCnt = 0;

void callBackCapture(ushort** ppImg, void* userArgs){
    try
    {
        int iWidth = instance->getWidth();
        int iHeight = instance->getHeight();
        int iDepth = *(int*)userArgs;

        qDebug() << "[INFO]: Received " << iDepth << " frames with dimensions " << iWidth << "x" << iHeight;

        for (int i = 0; i < iDepth; ++i){
            memcpy(instance->currentBuffer, ppImg[i], sizeof(SpectrumLogic::ushort) * iWidth * iHeight);
            QImage qImage((uchar*)instance->currentBuffer, iWidth, iHeight, QImage::Format_Grayscale16);
            qImage.save(instance->getSavingPath() + std::to_string(i).c_str() + ".tiff");
        }

    }
    catch (...)
    {
        qDebug()<< "[ERROR]: Exception in SaveStack_callback.";
    }
}

// For continuous Grabbing
void callBackLive(SpectrumLogic::ushort *pImg, int *pWidth, int *pHeight, SpectrumLogic::SLError *err, void *userArgs) {
    if(!instance->isRunning){
        qDebug() << "Interrupted by running flag.";
        return;
    }
    if (*err != SpectrumLogic::SLError::SL_ERROR_SUCCESS){
        qDebug() << "[ERROR]: Failed to get frame" ;
        instance->stopGrabbing();
        return;
    }

    int* pFrameCount = (int*)userArgs;
    qDebug() << "Frame count : " << *pFrameCount;

    memcpy(instance->currentBuffer, pImg, sizeof(SpectrumLogic::ushort) * *pWidth * *pHeight);
    emit instance->sendBuffer(instance->currentBuffer);

    QImage qImage((uchar*)instance->currentBuffer, *pWidth, *pHeight, QImage::Format_Grayscale16);
    emit instance->sendImage(qImage);


    if(instance->isSaveMode()){
        qDebug() << "entranced to saving mode frame cnt " << *pFrameCount;
        qImage.save(instance->getSavingPath() + std::to_string(*pFrameCount).c_str() + ".tiff");
    }

    ++*pFrameCount;
    if(loopCnt == 0 ){}
    else if(*pFrameCount == loopCnt){
        qDebug() << "Sequential grabbing finished.";
        instance->stopGrabbing();
        instance->setSaveMode(false);
        return;
    }
}



Detector::Detector() {
    dialog = new DetectorDialog;
    dialog->setDetector(this);
    if(currentBuffer != nullptr) free(currentBuffer);
    connect(this, &Detector::updateInformation, dialog, &DetectorDialog::updateInformation);
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
    setExposureMode(SpectrumLogic::ExposureModes::seq_mode);
    sl_device.SetNumberOfFrames(numFrame);

    loopCnt = numFrame;
    isRunning = true;

    startFrameNum = 0;
    currentBuffer = (unsigned short*)malloc((size_t)(getWidth()*getHeight()*2));
    qDebug() << "Try to start sequentialGrabbing : " << SpectrumLogic::SLErrorToString(sl_device.GoLive(callBackLive, &startFrameNum)).c_str();
    qDebug() << "Software Trigger : " << SpectrumLogic::SLErrorToString(sl_device.SoftwareTrigger()).c_str();
}

void Detector::continuousGrabbing()
{
    loopCnt = 0;
    isRunning =true;

    startFrameNum = 0;
    currentBuffer = (unsigned short*)malloc((size_t)(getWidth()*getHeight()*2));
    qDebug() << "Try to start continuousGrabbing : " << SpectrumLogic::SLErrorToString(sl_device.GoLive(callBackLive, &startFrameNum)).c_str();

}

void Detector::stopGrabbing() {
    isRunning = false;
    if(!sl_device.IsConnected()){
        qDebug() << "SL Device is not connected. Nothing will be changed.";
        return;
    }
    auto out = sl_device.GoUnLive();
    qDebug() << "Stop grabbing is called. " << SpectrumLogic::SLErrorToString(out).c_str();
    if(out != SpectrumLogic::SLError::SL_ERROR_SUCCESS){
        Sleep(1000);
        qDebug() << "We're going to try to stop it once again." << SpectrumLogic::SLErrorToString(sl_device.GoUnLive(true)).c_str();
    }

    if(currentBuffer != nullptr) free(currentBuffer);
    currentBuffer = nullptr;
}

void Detector::saveImages(int cnt){
    qDebug() << "Starting to save frames.";
    int numFrames = cnt;
    int expTime = getExposureTime();

    qDebug()  << "[INFO]: Exposure time: " << expTime << " ms"  << "[INFO]: Frames: " << numFrames ;

    if (sl_device.CaptureImgCallback(expTime, numFrames, callBackCapture) != SpectrumLogic::SLError::SL_ERROR_SUCCESS)
        qDebug() << "[ERROR]: CaptureImgCallback failed." ;
    else
        qDebug() << "[INFO]: CaptureImgCallback succeeded." ;
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
    //    updateInformation();
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
    //    updateInformation();
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
    //    updateInformation();
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
    //    updateInformation();
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
    qDebug() << "Set exposure time : " << SpectrumLogic::SLErrorToString(error).c_str() << "Value" << ms;
    if(error != SpectrumLogic::SLError::SL_ERROR_SUCCESS) return false;

    exposureTime = ms;
    //    updateInformation();
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
    if(!sl_device.IsConnected()) return false;
    auto error = sl_device.SetExposureMode(mode);
    qDebug() << "Set exposure mode :" << SpectrumLogic::SLErrorToString(error).c_str();
    //    updateInformation();
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
    //    updateInformation();
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
    //    updateInformation();
    if(error != SpectrumLogic::SLError::SL_ERROR_SUCCESS){
        return false;
    }else return true;
}

SpectrumLogic::FullWellModes Detector::getFullWellMode()
{

    return SpectrumLogic::FullWellModes::Low;
}
