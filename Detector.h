#ifndef DETECTOR_H
#define DETECTOR_H

#include "SLDevice.h"
#include "SLImage.h"
#include <iostream>
#include <QObject>

class Detector : public QObject
{
    Q_OBJECT
public:
    Detector() {
    }
    ~Detector(){}
    bool init(){
        std::cout << "initializing SpectrumLogic\n";
        sl_device = SpectrumLogic::SLDevice(sl_iFace);

        if (sl_device.OpenCamera() != SpectrumLogic::SLError::SL_ERROR_SUCCESS) {
            std::cout << "Failed to open a SL camera.\n";
            return false;
        }
        else std::cout << "Opened a SL camera.";
        return true;
    }
    static void callBackCapture(SpectrumLogic::ushort** pImg, void* userArgs) {

    }
    void grabCapture(int exposureTime, int numFrames) {
        std::cout << "Exp :" << exposureTime << ", numFrames : " << numFrames << "\n";

        if (sl_device.IsConnected()) {
            sl_device.CaptureImgCallback(exposureTime, numFrames, callBackCapture);
        }
        else std::cout << "SL Device isn't connected.";
    }
    static void callBackLive(SpectrumLogic::ushort* pImg, int* pWidth, int* pHeight, SpectrumLogic::SLError* err, void* userArgs) {
        int* pFrameCount = (int*)userArgs;

        if (*err != SpectrumLogic::SLError::SL_ERROR_SUCCESS)
        {
            std::cout << "[ERROR]: Failed to get frame" << std::endl;
            return;
        }

        std::cout << "[INFO]: Caught frame #" << *pFrameCount + 1 << " with dimensions " << *pWidth << " x " << *pHeight << std::endl;
        SpectrumLogic::SLImage image;
        image.Build(*pWidth, *pHeight, 1);
        memcpy(image.GetDataPointer(0), pImg, sizeof(SpectrumLogic::ushort) * *pWidth * *pHeight);

        if (!image.WriteTiffImage("C:/GoLiveCallbackCapture" + std::to_string(*pFrameCount + 1) + ".tif", image, 16))
            std::cout << "[ERROR]: Failed to save frame #" << *pFrameCount + 1 << std::endl;

        ++*pFrameCount;


    }
    void grabLive(int exposureTime, int frameCount) {
        std::cout << "Exp :" << exposureTime << ", FrameCount : " << frameCount << "\n";

        sl_device.SetExposureTime(exposureTime);
        sl_device.GoLive(callBackLive, &frameCount);
    }
    void stopGrabLive() {
        sl_device.GoUnLive();
    }

signals:
    void transfer();

private:
    SpectrumLogic::SLDevice sl_device;
    SpectrumLogic::DeviceInterface sl_iFace{ SpectrumLogic::S2I_GIGE };
};
#endif // DETECTOR_H
