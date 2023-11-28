#include "Grabber.h"
#include <QTime>
Grabber* instance = nullptr;
int callbackFromGrabber(frameindex_t picNr, void *){
    qDebug() << "[Callback]get into the grabber callback function.";
    uchar *buffer = (uchar*)Fg_getImagePtrEx(instance->getFg(), picNr, 0, instance->getDMAOut());
    QImage outputImage = QImage(buffer, instance->getImageWidth(), instance->getImageHeight(), QImage::Format_Grayscale16);

    emit instance->sendImage(outputImage);
    //    qDebug() << instance->timer->elapsed();
    //    qDebug() << "Callback called a t" << picNr << "My intended" << instance->getSequentialNumFrame();
    if(!instance->isContinuous() && (instance->getSequentialNumFrame() != 0)){
        if(picNr == instance->getSequentialNumFrame()){
            qDebug() << "Called stop grabbing";
            instance->stopGrabbing();
            instance->isRunning = false;
        }
    }
    return 0;
}

Grabber::Grabber(QObject *parent)
    : QObject{parent}
{
    instance = this;
    timer = new QElapsedTimer;
    dialog = new GrabberDialog;
    dialog->setGrabber(this);
//    connect(this, &Grabber::updateInformation, dialog, &GrabberDialog::updateInformation);
}

Grabber::~Grabber()
{
    Fg_FreeMemEx(currentFg, DMAInverse);
    Fg_FreeMemEx(currentFg, DMAOut);

    free(imgBuffer);
    free(outBuffer);

    Fg_FreeGrabber(currentFg);
}

bool Grabber::loadApplet(QString path)
{
    currentFg = Fg_Init(path.toStdString().c_str(), 0);
    if(currentFg == 0){
        qDebug() << ("Failed to load an applet file." + QString(Fg_getLastErrorDescription(currentFg)));
        return false;
    }
    qDebug() << path << "is loaded.";
    return true;
}

bool Grabber::loadConfiguration(QString path)
{
    if(currentFg == nullptr){
        qDebug() << ("Grabber is not initialized.");
        return false;
    }
    auto error = Fg_loadConfig(currentFg, path.toStdString().c_str());
    if(0 > error){
        qDebug() << ("Grabber couldn't load a configuration file. " + QString(Fg_getErrorDescription(currentFg, error)));
        return false;
    }
    qDebug() << (path + " is loaded.");
    return true;
}

void Grabber::initialize()
{
    vaDevice = SHalInitDevice(0);

    size_t dmaLen = getDMALength();
    DMAInverse = Fg_AllocMemHead(currentFg, dmaLen, 1);
    imgBuffer = (unsigned short*)malloc((size_t)dmaLen);
    Fg_AddMem(currentFg, imgBuffer, (size_t)dmaLen, 0, DMAInverse);

    DMAOut = Fg_AllocMemHead(currentFg, dmaLen, 1);
    outBuffer = (unsigned short*)malloc((size_t)dmaLen);
    Fg_AddMem(currentFg, outBuffer, (size_t)dmaLen, 0, DMAOut);

    auto apcData = new APC;
    apcData->ctrl.version = 0;
    apcData->ctrl.data = &apcData;
    apcData->ctrl.func = callbackFromGrabber;
    apcData->ctrl.flags = FG_APC_DEFAULTS | FG_APC_IGNORE_STOP | FG_APC_IGNORE_TIMEOUTS | FG_APC_HIGH_PRIORITY;
    apcData->ctrl.timeout = 500000000;
    Fg_registerApcHandler(currentFg, 0, &apcData->ctrl, FG_APC_CONTROL_BASIC);

}

int Grabber::getDMALength()
{
    return getImageWidth()*getImageHeight()*bytesperpixel;
}

bool Grabber::setOutWidth(int _w){

    width = _w;
    auto error = Fg_setParameter(currentFg, FG_WIDTH, &width, 0);
    if(error != 0 ){
        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
        return false;
    }
    emit updateInformation();
    return true;
}

int Grabber::getOutWidth(){
    int value=0;
    Fg_getParameter(currentFg, FG_WIDTH, (void*)&value, 0);
    return value;
}

bool Grabber::setOutHeight(int _h){
    height = _h;
    auto error = Fg_setParameter(currentFg, FG_HEIGHT, &height, 0);
    if(error != 0 ){
        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
        return false;
    }
    emit updateInformation();
    return true;
}

int Grabber::getOutHeight(){
    int value=0;
    Fg_getParameter(currentFg, FG_HEIGHT, (void*)&value, 0);
    return value;
}

bool Grabber::setXOffset(int _x)
{
    auto error = Fg_setParameter(currentFg, FG_XOFFSET, &_x,0);
    if(error != 0 ){
        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
        return false;
    }
    emit updateInformation();
    return true;
}

int Grabber::getXOffset()
{
    int value=0;
    Fg_getParameter(currentFg, FG_XOFFSET, &value, 0);
    return value;
}

bool Grabber::setYOffset(int _y)
{
    auto error = Fg_setParameter(currentFg, FG_YOFFSET, &_y,0);
    if(error != 0 ){
        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
        return false;
    }
    emit updateInformation();
    return true;
}

int Grabber::getYOffset()
{
    int value=0;
    Fg_getParameter(currentFg, FG_YOFFSET, &value, 0);
    return value;
}

bool Grabber::setImageWidth(int _w)
{

//    Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_EasyRamLUT_IS_GreaterEqual_Number"), _w, 0);
//    Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_SplitLine_LineLength"), _w*2, 0); // Split line should be the double of the width.
//    Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_Cal_Data_XLength"), ceil(_w / 12), 0);
//    auto error = Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ROI_X_Length"), _w, 0);
//    if(error != 0 ){
//        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
//        return false;
//    }
//    emit updateInformation();
    return true;
}

bool Grabber::setImageHeight(int _h)
{
//    Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_EasyRamLUT_CreateBlankImage_ImageHeight"), _h, 0);
//    auto error = Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ROI_Y_Length"), _h, 0);
//    if(error != 0 ){
//        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
//        return false;
//    }
//    emit updateInformation();
    return true;
}

int Grabber::getImageWidth()
{
    int value=0;
    Fg_getParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ROI_X_Length"), &value, 0);
    return value;
}

int Grabber::getImageHeight()
{
    int value=0;
    Fg_getParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ROI_Y_Length"), &value, 0);
    return value;
}


bool Grabber::setShadingCorrectionEnable(bool on)
{
    auto error = Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_Shading_Enable_Value"), on, 0);
    if(error != 0 ){
        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
        return false;
    }
    emit updateInformation();
    return true;
}

bool Grabber::getShadingCorrectionEnable()
{
    int value=0;
    Fg_getParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_Shading_Enable_Value"), &value, 0);
    return value;
}//Device1_Process0_Implementation_DeadPixelInterpolation_Enable_Value
bool Grabber::setDeadPixelInterpolation(bool on)
{
    auto error = Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_DeadPixelInterpolation_Enable_Value"), on, 0);
    if(error != 0 ){
        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
        return false;
    }
    emit updateInformation();
    return true;
}

bool Grabber::getDeadPixelInterpolation()
{
    int value=0;
    Fg_getParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_DeadPixelInterpolation_Enable_Value"), &value, 0);
    return value;
}
//Device1_Process0_Implementation_Remove_Dark_Line_H_Enable_Value
bool Grabber::setRemovingDarkH(bool on)
{
    auto error = Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_Remove_Dark_Line_H_Enable_Value"), on, 0);
    if(error != 0 ){
        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
        return false;
    }
    emit updateInformation();
    return true;
}

bool Grabber::getRemovingDarkH()
{
    int value=0;
    Fg_getParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_Remove_Dark_Line_H_Enable_Value"), &value, 0);
    return value;
}
//Device1_Process0_Implementation_Remove_Dark_Line_Enable_Value
bool Grabber::setRemovingDarkV(bool on)
{
    auto error = Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_Remove_Dark_Line_Enable_Value"), on, 0);
    if(error != 0 ){
        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
        return false;
    }
    emit updateInformation();
    return true;
}

bool Grabber::getRemovingDarkV()
{
    int value=0;
    Fg_getParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_Remove_Dark_Line_Enable_Value"), &value, 0);
    return value;
}
// Device1_Process0_Implementation_ShadingCorrection_Shading_OverSaturation_Value=1200;
bool Grabber::setOverSaturation(int _v)
{
    auto error =Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_Shading_OverSaturation_Value"), _v, 0);
    if(error != 0 ){
        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
        return false;
    }
    emit updateInformation();
    return true;
}

int Grabber::getOverSaturation()
{
    int value=0;
    Fg_getParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_Shading_OverSaturation_Value"), &value, 0);
    return value;
}
//Device1_Process0_Implementation_ShadingCorrection_EasyRamLUT_mE6RamLUT_InitFileName
bool Grabber::setLUTFileName(QString fileName)
{
    auto error = Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_EasyRamLUT_mE6RamLUT_InitFileName"), fileName.toStdString(), 0);
    if(error != 0 ){
        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
        return false;
    }
    emit updateInformation();
    return true;
}

QString Grabber::getLUTFileName()
{
    std::string value;
    Fg_getParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_EasyRamLUT_mE6RamLUT_InitFileName"), value, 0);

    return QString::fromStdString(value);
}

//Device1_Process0_Implementation_ShadingCorrection_EasyRamLUT_mE6RamLUT_LoadInitFile=1;
bool Grabber::setInitFile(bool on)
{
    auto error = Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_EasyRamLUT_mE6RamLUT_LoadInitFile"), on, 0);
    if(error != 0 ){
        qDebug() << "Error :" << Fg_getErrorDescription(currentFg, error);
        return false;
    }
    emit updateInformation();
    return true;

}
bool Grabber::getInitFile()
{
    int value=0;
    Fg_getParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_EasyRamLUT_mE6RamLUT_LoadInitFile"), &value, 0);
    return value;
}

int Grabber::getParameterId(QString parameter)
{
    return Fg_getParameterIdByName(currentFg, parameter.toStdString().c_str());
}

void Grabber::setCalibMode(bool on)
{
    Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_Shading_Enable_Value"), on, 0);
    Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_DeadPixelInterpolation_Enable_Value"), on, 0);
    Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_Remove_Dark_Line_Enable_Value"), on, 0);
    Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_Remove_Dark_Line_H_Enable_Value"), on, 0);
    Sleep(100);
    emit updateInformation();

}

void Grabber::sequentialGrabbing(int numFrame)
{
    isRunning = true;
    setContinuous(false);
    sequentialNumFrame = numFrame;
    auto a = Fg_AcquireEx(currentFg, 0, numFrame, ACQ_STANDARD, DMAOut);
    auto b= Fg_AcquireEx(currentFg, 1, numFrame+1, ACQ_STANDARD, DMAInverse);
    qDebug() << "Sequential Grabbing command result DMA0:" << Fg_getErrorDescription(currentFg, a) << "DMA1:" << Fg_getErrorDescription(currentFg,b);
}

void Grabber::continuousGrabbing()
{
    isRunning = true;
    setContinuous(true);
    sequentialNumFrame = 0;
    auto a = Fg_AcquireEx(currentFg, 0, GRAB_INFINITE, ACQ_STANDARD, DMAOut);
    auto b= Fg_AcquireEx(currentFg, 1, GRAB_INFINITE, ACQ_STANDARD, DMAInverse);
    qDebug() << "Continuous Grabbing command result DMA0:" << Fg_getErrorDescription(currentFg, a) << "DMA1:" << Fg_getErrorDescription(currentFg,b);

}

void Grabber::stopGrabbing()
{
    isRunning = false;
    sequentialNumFrame = 0;
    setContinuous(false);

    auto a = Fg_stopAcquireEx(currentFg, 0, DMAOut, 0);
    auto b = Fg_stopAcquireEx(currentFg, 1, DMAInverse, 0);
    qDebug() << "Stop Grabbing command result DMA0:" << Fg_getErrorDescription(currentFg, a) << "DMA1:" << Fg_getErrorDescription(currentFg,b);
}

void Grabber::saveConfig(QString path)
{
    Fg_saveConfig(currentFg, (path + ".mcf").toStdString().c_str());
}

void Grabber::convertToGrabberImage(unsigned short *buffer)
{
    if(isRunning){
        timer->restart();
        size_t dmaLen = getDMALength();
        memcpy(imgBuffer, buffer, dmaLen);
        auto error = SHalSetBufferStatus(vaDevice, DMAInverse, ((dmaLen / 4) << 32) | 0, FG_SELECT_BUFFER );
        int frameNum = Fg_getLastPicNumberBlockingEx(currentFg, 1, 1, 10, DMAInverse);
        qDebug() << "Convertintg : " << error << frameNum;
        if(error != 0) qDebug() << "Set buffer status error: " << error << Fg_getErrorDescription(currentFg, error);
    }
}

