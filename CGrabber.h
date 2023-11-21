#ifndef CGRABBER_H
#define CGRABBER_H

#include "basler_fg.h"
#include "fg_struct.h"
#include "sisoIo.h"
#include "siso_hal.h"
#include "CGrabberDialog.h"

#include <QObject>
#include <QImage>
#include <QDebug>
#include <QElapsedTimer>

struct APC {
    FgApcControl ctrl;
    unsigned int dmaIndex;
    dma_mem *memBuf;
    int width;
    int height;
};
class CGrabber : public QObject
{
    Q_OBJECT
public:
    explicit CGrabber(QObject *parent = nullptr);
    ~CGrabber();
    bool loadApplet(QString path);
    bool loadConfiguration(QString path);
    void initialize();
    int getDMALength();

    bool setOutWidth(int _w);
    int getOutWidth();
    bool setOutHeight(int _h);
    int getOutHeight();
    bool setXOffset(int _x);
    int getXOffset();
    bool setYOffset(int _y);
    int getYOffset();

    bool setImageWidth(int _w);
    bool setImageHeight(int _h);
    int getImageWidth();
    int getImageHeight();

    bool setShadingCorrectionEnable(bool on);
    bool getShadingCorrectionEnable();

    bool setDeadPixelInterpolation(bool on);
    bool getDeadPixelInterpolation();

    bool setRemovingDarkH(bool on);
    bool getRemovingDarkH();

    bool setRemovingDarkV(bool on);
    bool getRemovingDarkV();





    bool setOverSaturation(int _v);
    int getOverSaturation();

    bool setLUTFileName(QString fileName);
    QString getLUTFileName();

    bool setInitFile(bool on);
    bool getInitFile();


    void setBytesPerPixel(int _b){ bytesperpixel = _b; }
    int getBytesPerPixel(){ return bytesperpixel; }
    Fg_Struct* getFg(){ return currentFg; }
    dma_mem* getDMAOut(){ return DMAOut; }
    int getParameterId(QString parameter);
    void setCalibMode(bool on);
    void sequentialGrabbing(int numFrame);
    void continuousGrabbing();
    void stopGrabbing();
    bool isContinuous(){ return continuousMode;}
    int getSequentialNumFrame(){ return sequentialNumFrame; }
    bool isRunning = false;
    QElapsedTimer *timer;
    QDialog *getDialog(){ return dynamic_cast<QDialog*>(dialog);}

public slots:
    void convertToGrabberImage(unsigned short *buffer);

signals:
    void updateInformation();
    void sendImage(const QImage &image);

private:
    Fg_Struct *currentFg = nullptr;
    struct VAdevice *vaDevice = nullptr;
    SisoIoImageEngine* imageHandle0;

    dma_mem *DMAInverse;
    unsigned short* imgBuffer;

    dma_mem *DMAOut;
    unsigned short* outBuffer;

    int width = 2400;
    int height = 600;
    int bytesperpixel = 2;

    void setContinuous(bool on){ continuousMode = on; }
    bool continuousMode = false;
    int sequentialNumFrame = 0;

    CGrabberDialog *dialog = nullptr;


};

#endif // CGRABBER_H
