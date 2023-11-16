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
    void loadApplet(QString path);
    void loadConfiguration(QString path);
    void initialize();
    int getDMALength();
    void setWidth(int _w);
    int getWidth(){ return width; }
    void setHeight(int _h);
    int getHeight(){ return height; }
    bool setROI(int width, int height);
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

public slots:
    void convertToGrabberImage(const unsigned short *buffer);

signals:
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
