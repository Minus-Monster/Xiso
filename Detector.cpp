#include "Detector.h"
#include "DetectorDialog.h"
#include <QImage>

Detector *instance = nullptr;
int setFrameCount =0;
int loopCnt = 0;

// For continuous Grabbing
void callBackLive(SpectrumLogic::ushort *pImg, int *pWidth, int *pHeight, SpectrumLogic::SLError *err, void *userArgs) {
    int* pFrameCount = (int*)userArgs;
    qDebug() << "frame count : " << *pFrameCount  << "Loopcnt" << loopCnt;
    if(!instance->isRunning) return;

    if (*err != SpectrumLogic::SLError::SL_ERROR_SUCCESS)
    {
        qDebug() << "[ERROR]: Failed to get frame" ;
        instance->stopGrabbing();
        return;
    }

    memcpy(instance->currentBuffer, pImg, sizeof(SpectrumLogic::ushort) * *pWidth * *pHeight);

    SpectrumLogic::SLImage image;
    image.Build(*pWidth, *pHeight, 1);
    if(instance->isSaveMode()){
        qDebug() << "entranced to saving mode frame cnt " << *pFrameCount;
        memcpy(image.GetDataPointer(0), pImg, sizeof(SpectrumLogic::ushort) * *pWidth * *pHeight);
        if (!image.WriteTiffImage(instance->getSavingPath().toStdString() + std::to_string(*pFrameCount) + ".tiff", image, 16)){
            qDebug() << "[ERROR]: Failed to save frame #" << *pFrameCount + 1;
        }
    }
    QImage qImage((uchar*)instance->currentBuffer, *pWidth, *pHeight, QImage::Format_Grayscale16);
    emit instance->sendBuffer(instance->currentBuffer);
    emit instance->sendImage(qImage);

    if(loopCnt == 0 ){}
    else if(*pFrameCount == loopCnt){
        instance->stopGrabbing();
        return;
    }
    ++*pFrameCount;
}

// For capturing Grabbing
void callBackCapture(unsigned short** ppImg, void* userArgs){
    qDebug() << "How many times is this function called when I'm just trying to get two shots";
    try{
        int cWidth = instance->getWidth();
        int cHeight = instance->getHeight();
        int bytePerPixel = 2;

        memcpy(instance->currentBuffer, ppImg[0], sizeof(unsigned short) * cWidth * cHeight);
        // Buffer all saved?

    }catch(...){
        qDebug() << "CallBackCapture error occured while creating an image";
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
    qDebug() << "Sequential Grabbing started with frame " << numFrame;
    if(!sl_device.IsConnected()){
        qDebug() << "SL Device is not connected. Nothing will be changed.";
        return;
    }
    setExposureMode(SpectrumLogic::ExposureModes::seq_mode);
    sl_device.SetNumberOfFrames(numFrame);

    setFrameCount = 0;
    loopCnt = numFrame;
    qDebug() << "Loopcnt is " << loopCnt;
    currentBuffer = (unsigned short*)malloc((size_t)(getWidth()*getHeight()*2));
    sl_device.GoLive(callBackLive, &setFrameCount);
    sl_device.SoftwareTrigger();
    isRunning = true;
}

void Detector::continuousGrabbing()
{
    setFrameCount = 0;
    loopCnt = 0;
    isRunning =true;
    currentBuffer = (unsigned short*)malloc((size_t)(getWidth()*getHeight()*2));
    setExposureMode(SpectrumLogic::ExposureModes::fps30_mode);
    sl_device.GoLive(callBackLive, &setFrameCount);
}

void Detector::stopGrabbing() {
    if(!sl_device.IsConnected()){
        qDebug() << "SL Device is not connected. Nothing will be changed.";
        return;
    }
    sl_device.GoUnLive();
    isRunning = false;
    if(currentBuffer != nullptr) free(currentBuffer);
    currentBuffer = nullptr;
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
