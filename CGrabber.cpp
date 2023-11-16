#include "CGrabber.h"

CGrabber* instance = nullptr;
int callbackFromGrabber(frameindex_t picNr, void *){
    uchar *buffer = (uchar*)Fg_getImagePtrEx(instance->getFg(), picNr, 0, instance->getDMAOut());
    QImage outputImage = QImage(buffer, instance->getWidth(), instance->getHeight(), QImage::Format_Grayscale16);
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

CGrabber::CGrabber(QObject *parent)
    : QObject{parent}
{
    instance = this;
    timer = new QElapsedTimer;
    dialog = new CGrabberDialog;
    dialog->show();
}

CGrabber::~CGrabber()
{
    Fg_FreeMemEx(currentFg, DMAInverse);
    Fg_FreeMemEx(currentFg, DMAOut);

    free(imgBuffer);
    free(outBuffer);

    Fg_FreeGrabber(currentFg);
}

void CGrabber::loadApplet(QString path)
{
    currentFg = Fg_Init(path.toStdString().c_str(), 0);
    if(currentFg == 0){
        qDebug() << ("Failed to load an applet file." + QString(Fg_getLastErrorDescription(currentFg)));
    }
    qDebug() << path << "is loaded.";
}

void CGrabber::loadConfiguration(QString path)
{
    if(currentFg == nullptr){
        qDebug() << ("Grabber is not initialized.");
    }
    auto error = Fg_loadConfig(currentFg, path.toStdString().c_str());
    if(0 > error){
        qDebug() << ("Grabber couldn't load a configuration file. " + QString(Fg_getErrorDescription(currentFg, error)));
    }
    qDebug() << (path + " is loaded");
}

void CGrabber::initialize()
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

int CGrabber::getDMALength()
{
    return width*height*bytesperpixel;
}

void CGrabber::setWidth(int _w){

    width = _w;

}

void CGrabber::setHeight(int _h){
    height = _h;
}

bool CGrabber::setROI(int width, int height){
    int a, b,c,d,e,f,g;
    a= Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_module133_X_Length"), width, 0);
    //YLegnth
    b=Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_module133_Y_Length"), height, 0);
    //Fg_setParameter(fg, FG_HEIGHT, &m_iHeight, 0);

    // Set Cal XLength
    unsigned int xLength = width /8;
    c=Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_Cal_Data_XLength"), xLength, 0);

    // Set Cal YLength
    unsigned int yLength = height;
    d=Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_Cal_Data_YLength"), yLength, 0);

    // Update ROI
    unsigned int nUpdateROI;
    nUpdateROI = 1;
    e=Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_Cal_Data_UpdateROI"), nUpdateROI, 0);

    f=Fg_setParameter(currentFg, FG_WIDTH, &width, 0);
    g=Fg_setParameter(currentFg, FG_HEIGHT, &height, 0);
    qDebug() << Fg_getErrorDescription(currentFg,a)<< Fg_getErrorDescription(currentFg,b) << Fg_getErrorDescription(currentFg,c) << Fg_getErrorDescription(currentFg,d) << Fg_getErrorDescription(currentFg,e) << Fg_getErrorDescription(currentFg,f) << Fg_getErrorDescription(currentFg,g);

    if(a || b || c || d || e || f || g){
        return false;
    }else return true;
}

int CGrabber::getParameterId(QString parameter)
{
    return Fg_getParameterIdByName(currentFg, parameter.toStdString().c_str());
}

void CGrabber::setCalibMode(bool on)
{
    Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_ShadingCorrection_Shading_Enable_Value"), on, 0);
    Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_DeadPixelInterpolation_Enable_Value"), on, 0);
    Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_Remove_Dark_Line_Enable_Value"), on, 0);
    Fg_setParameterWithType(currentFg, getParameterId("Device1_Process0_Implementation_Remove_Dark_Line_H_Enable_Value"), on, 0);
    Sleep(100);
}

void CGrabber::sequentialGrabbing(int numFrame)
{
    isRunning = true;
    setContinuous(false);
    sequentialNumFrame = numFrame;
    auto a = Fg_AcquireEx(currentFg, 0, numFrame, ACQ_STANDARD, DMAOut);
    auto b= Fg_AcquireEx(currentFg, 1, numFrame+1, ACQ_STANDARD, DMAInverse);
    qDebug() << "Grabbing result : " << a << "and" << b;
}

void CGrabber::continuousGrabbing()
{
    isRunning = true;
    setContinuous(true);
    sequentialNumFrame = 0;
    auto a = Fg_AcquireEx(currentFg, 0, GRAB_INFINITE, ACQ_STANDARD, DMAOut);
    auto b= Fg_AcquireEx(currentFg, 1, GRAB_INFINITE, ACQ_STANDARD, DMAInverse);
    qDebug() << "Grabbing result : " << a << "and" << b;

}

void CGrabber::stopGrabbing()
{
    isRunning = false;
    sequentialNumFrame = 0;
    setContinuous(false);

    auto a = Fg_stopAcquireEx(currentFg, 0, DMAOut, 0);
    auto b = Fg_stopAcquireEx(currentFg, 1, DMAInverse, 0);
    qDebug() << "Stop grabbing result : " << a << "and" << b;
}

void CGrabber::convertToGrabberImage(const unsigned short *buffer)
{
    if(isRunning){
        timer->restart();
        size_t dmaLen = getDMALength();
        memcpy(imgBuffer, buffer, dmaLen);
        auto error = SHalSetBufferStatus(vaDevice, DMAInverse, ((dmaLen / 4) << 32) | 0, FG_SELECT_BUFFER );
        if(error != 0) qDebug() << Fg_getErrorDescription(currentFg, error);
    }
}

