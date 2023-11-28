#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <fstream>
#include <QTextBlock>
#include <QTextCursor>
#include <QMetaObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QElapsedTimer;
    layout = new QVBoxLayout;
    widget = new Qylon::GraphicsWidget;
    widget->initialize();
    layout->addWidget(widget);
    ui->frame->setLayout(layout);

    bcControl = new BrightContrastControl;
    //    bcControl->show();


    connect(ui->actionPanel, &QAction::triggered, ui->dockWidget_panel, &QDockWidget::setVisible);
    connect(ui->actionDebug_Console, &QAction::triggered, ui->dockWidget_debug, &QDockWidget::setVisible);
    connect(ui->actionDetector_Settings, &QAction::triggered, ui->dockWidget_detector, &QDockWidget::setVisible);
    connect(ui->actionGrabber_Settings, &QAction::triggered, ui->dockWidget_grabber, &QDockWidget::setVisible);
    // Sequential grabbing
    connect(ui->pushButton_Sequential, &QPushButton::clicked, this, [this](){
        int cnt = ui->spinBox_Sequential->value();
        this->detector->setExposureMode(SpectrumLogic::ExposureModes::seq_mode);
        timer->start();
        emit grabbingStart(cnt);

        //                this->grabber->continuousGrabbing();
        //                QImage *img = new QImage("C:/Users/minwoo/Desktop/Minu/CalTest/Test_Raw.tiff");
        //                this->grabber->convertToGrabberImage((unsigned short*)img->bits());

    });
    // Continuous grabbing
    connect(ui->pushButton_Continuous, &QPushButton::clicked, this, [this](bool on){
        if(on) emit grabbingStart();
        else emit grabbingFinished();
        timer->start();
    });
    // Reset
    connect(ui->pushButton_reset, &QPushButton::clicked, this, [this](){
        emit grabbingFinished();
        timer->elapsed();
    });
    // Dark calib
    connect(ui->pushButton_dark, &QPushButton::clicked, this, [this](){
        QString path = QFileDialog::getExistingDirectory(this, "Select a directory to save", QDir::homePath());
        if(path == "") return;
        darkCalPath = path;
        grabber->setCalibMode(false);
        // Grabbing x images
        detector->setSaveMode(true);
        detector->setSavingPath(path + "/DarkRaw");
        detector->sequentialGrabbing(ui->spinBox_calibCount->value());
        // need to capture X frames and save that frames in the specific folder.

        grabber->setCalibMode(true);
        //        detector->setSaveMode(false); // need to revise this code. threading error.
    });
    // bright calib
    connect(ui->pushButton_bright, &QPushButton::clicked, this, [this](){
        QString path = QFileDialog::getExistingDirectory(this, "Select a directory to save", QDir::homePath());
        if(path == "") return;
        brightCalPath = path;

        grabber->setCalibMode(false);
        // Grabbing x images
        detector->setSaveMode(true);
        detector->setSavingPath(path + "/WhiteRaw");
        detector->sequentialGrabbing(ui->spinBox_calibCount->value());

        grabber->setCalibMode(true);
        //        detector->setSaveMode(false);
    });
    // calib out
    connect(ui->pushButton_out, &QPushButton::clicked, this, [this](){
        detector->setSaveMode(false);
        // Test Function now
        darkCalPath = "C:/Users/minwoo/Desktop/Minu/Xiso/Test/Dark";
        brightCalPath = "C:/Users/minwoo/Desktop/Minu/Xiso/Test/Bright";
        if(darkCalPath == "" || brightCalPath ==  ""){
            qDebug() << "Cal path is not set";
            return;
        }
        auto darkList = QDir(darkCalPath).entryList(QStringList() << "*.tif" << "*.tiff", QDir::Files);
        auto brightList = QDir(brightCalPath).entryList(QStringList() << "*.tif" << "*.tiff", QDir::Files);

        int cWidth = QImage(darkCalPath + "/" + darkList.first()).width();
        int cHeight = QImage(darkCalPath + "/" + darkList.first()).height();

        QVector<QImage> images;
        foreach(const QString &imagePath, darkList){
            QString currentImagePath = darkCalPath + "/" + imagePath;
            QImage currentImage(currentImagePath);
            images.append(currentImage);
            qDebug() << "Appended" << currentImagePath;
        }
        QImage averageImage = QImage(cWidth, cHeight, QImage::Format_Grayscale16);
        averageImage.fill(0);
        for(int y=0; y < averageImage.height(); ++y){
            for(int x=0; x < averageImage.width(); ++x){
                qint64 sum = 0;
                for(const QImage& current : images){
                    sum += qRed(current.pixel(x,y));
                }
                qint64 average = sum / images.size();
                averageImage.setPixel(x,y, qRgb(average, average, average));
            }
        }
        averageImage.save(darkCalPath +"/average.tiff");

        //        QImage front(outPixel.width(), outPixel.height(), QImage::Format::Format_Grayscale8);
        //        QImage back(outPixel.width(), outPixel.height(), QImage::Format::Format_Grayscale8);
        //        qDebug() << front << back << outPixel;
        //        for(int y=0; y < outPixel.height(); ++y){
        //            for(int x=0; x <outPixel.width(); ++x){
        //                QRgb pixelVal = outPixel.pixel(x,y);

        //                uchar frontBits = static_cast<uchar>((pixelVal >> 8) & 0xFF);
        //                front.setPixel(x,y, qRgb(frontBits, frontBits, frontBits));

        //                uchar backBits = static_cast<uchar>(pixelVal & 0xFF);
        //                back.setPixel(x,y, qRgb(backBits, backBits, backBits));
        //            }
        //        }
        //        front.save(darkCalPath +"/FB.tiff");
        //        back.save(darkCalPath +"/BB.tiff");


        /*
        SisoIoImageEngine* imageHandle0 = NULL;
        int m_iWidth = this->ui->spinBox_width->value(); // Need to edit
        int m_iHeight = this->ui->spinBox_height->value(); // Need to edit
        int nByte = 2;
        int nAvgCount = ui->spinBox_calibCount->value();

        darkCalPath = "C:/Users/User/Desktop/Minu/Xiso/Cal";
        brightCalPath = "C:/Users/User/Desktop/Minu/Xiso/Cal";
        cal_Data0 = new unsigned char[(long long)m_iWidth*m_iHeight];
        cal_Data1 = new unsigned char[(long long)m_iWidth*m_iHeight];
        cal_Data2 = new unsigned char[(long long)m_iWidth*m_iHeight];
        cal_Data3 = new unsigned char[(long long)m_iWidth*m_iHeight];
        unsigned short* Temp;// = new unsigned short[m_iWidth * m_iHeight * nByte];
        //        memset(Temp, 0, m_iWidth * m_iHeight * nByte);
        unsigned short* Temp1 = new unsigned short[m_iWidth * m_iHeight * nByte];
        memset(Temp1, 0, m_iWidth * m_iHeight * nByte);
        unsigned short* Temp2 = new unsigned short[m_iWidth * m_iHeight * nByte];
        memset(Temp2, 0, m_iWidth * m_iHeight * nByte);
        unsigned short* Temp3 = new unsigned short[m_iWidth * m_iHeight * nByte];
        memset(Temp3, 0, m_iWidth * m_iHeight * nByte);
        unsigned short* Temp4 = new unsigned short[m_iWidth * m_iHeight * nByte];
        memset(Temp4, 0, m_iWidth * m_iHeight * nByte);
        unsigned short* Temp5 = new unsigned short[m_iWidth * m_iHeight * nByte];
        memset(Temp5, 0, m_iWidth * m_iHeight * nByte);
        unsigned short* Temp6 = new unsigned short[m_iWidth * m_iHeight * nByte];
        memset(Temp6, 0, m_iWidth * m_iHeight * nByte);
        unsigned short* Temp7 = new unsigned short[m_iWidth * m_iHeight * nByte];
        memset(Temp7, 0, m_iWidth * m_iHeight * nByte);
        unsigned short* Temp8 = new unsigned short[m_iWidth * m_iHeight * nByte];
        memset(Temp8, 0, m_iWidth * m_iHeight * nByte);
        unsigned short* AveImg = new unsigned short[m_iWidth * m_iHeight * nByte];
        memset(AveImg, 0, m_iWidth * m_iHeight * nByte);

        qDebug() << nAvgCount;
        for (int n = 0; n < nAvgCount; n++){
            // convert the path
            QString originalPath = (darkCalPath + "/DarkRaw" + QString::number(n) + ".tiff");
            auto error = IoImageOpen(originalPath.toStdString().c_str(), &imageHandle0);
            if(error == 0){
                qDebug() << "Error";
                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++){
                    for (int j = 0; j < m_iWidth; j++){
                        if(n < 2){
                            Temp1[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>=2 && n < 4){
                            Temp2[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>=4 && n < 6){
                            Temp3[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>=6 && n < 8){
                            Temp4[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>=8 && n < 10){
                            Temp5[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>=10 && n < 12){
                            Temp6[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>= 12 && n < 14){
                            Temp7[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>= 14 && n < 16){
                            Temp8[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else{
                            qDebug() << "Out of index";
                        }
                    }
                }
            }else{
                qDebug() << Fg_getErrorDescription(nullptr, error);
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;

        for (int i = 0; i < m_iHeight; i++){
            for (int j = 0; j < m_iWidth; j++){
                AveImg[i * m_iWidth + j] = ((Temp1[i * m_iWidth + j]) + (Temp2[i * m_iWidth + j]) + (Temp3[i * m_iWidth + j])+(Temp4[i * m_iWidth + j])+ (Temp5[i * m_iWidth + j])+ (Temp6[i * m_iWidth + j])+ (Temp7[i * m_iWidth + j])+ (Temp8[i * m_iWidth + j])) / nAvgCount;
            }
        }

        IoWriteTiff( (darkCalPath + "/DcalAvg.tiff").toStdString().c_str(), (unsigned char*)AveImg, m_iWidth, m_iHeight, 16, 1);
        for (int i = 0; i < m_iHeight; i++){
            for (int j = 0; j < m_iWidth; j++){
                // Low 8bit
                cal_Data0[i * m_iWidth + j] = AveImg[i * m_iWidth + j] / 1;   //BIT SHIFT 2^8=256 ->  (Bit7 ~ Bit0)
                // High 8bit
                cal_Data1[i * m_iWidth + j] = AveImg[i * m_iWidth + j] / 256;   //BIT SHIFT 2^8=256 ->  (Bit15 ~ Bit8)
            }
        }
        IoWriteTiff((darkCalPath + "/D1.tiff").toStdString().c_str(), cal_Data0, m_iWidth, m_iHeight, 8, 1);
        IoWriteTiff((darkCalPath + "/D2.tiff").toStdString().c_str(), cal_Data1, m_iWidth, m_iHeight, 8, 1);

        // WhiteCal

        int error = 0;

        for (int n = 0; n < nAvgCount; n++){
            error = IoImageOpen((brightCalPath + "/WhiteRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
            if (error == 0){
                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++){
                    for (int j = 0; j < m_iWidth; j++){
                        if(n < 2){
                            Temp1[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>=2 && n < 4){
                            Temp2[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>=4 && n < 6){
                            Temp3[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>=6 && n < 8){
                            Temp4[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>=8 && n < 10){
                            Temp5[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>=10 && n < 12){
                            Temp6[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>= 12 && n < 14){
                            Temp7[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else if(n>= 14 && n < 16){
                            Temp8[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                        }else{
                            qDebug() << "Out of index";
                        }
                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;

        for (int i = 0; i < m_iHeight; i++){
            for (int j = 0; j < m_iWidth; j++){
                AveImg[i * m_iWidth + j] = ((Temp1[i * m_iWidth + j]) + (Temp2[i * m_iWidth + j]) + (Temp3[i * m_iWidth + j])+(Temp4[i * m_iWidth + j])+ (Temp5[i * m_iWidth + j])+ (Temp6[i * m_iWidth + j])+ (Temp7[i * m_iWidth + j])+ (Temp8[i * m_iWidth + j])) / nAvgCount;
            }
        }
        IoWriteTiff((brightCalPath + "/WcalAvg.tiff").toStdString().c_str(), (unsigned char*)AveImg, m_iWidth, m_iHeight, 16, 1);
        //        IoWriteTiff("C:/Users/User/Desktop/Minu/Xiso/Test/Wcal/DcalAvg.tiff", (unsigned char*)AveImg, m_iWidth, m_iHeight, 16, 1);

        for (int i = 0; i < m_iHeight; i++){
            for (int j = 0; j < m_iWidth; j++){
                // Low 8bit
                cal_Data2[i * m_iWidth + j] = AveImg[i * m_iWidth + j] / 1;   //BIT SHIFT 2^8=256 ->  (Bit7 ~ Bit0)
                // High 8bit
                cal_Data3[i * m_iWidth + j] = AveImg[i * m_iWidth + j] / 256;   //BIT SHIFT 2^8=256 ->  (Bit15 ~ Bit8)
            }
        }
        IoWriteTiff((brightCalPath + "/B1.tiff").toStdString().c_str(), cal_Data2, m_iWidth, m_iHeight, 8, 1);
        IoWriteTiff((brightCalPath + "/B2.tiff").toStdString().c_str(), cal_Data3, m_iWidth, m_iHeight, 8, 1);

        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;

        delete []AveImg;
        delete []Temp8;
        delete []Temp7;
        delete []Temp6;
        delete []Temp5;
        delete []Temp4;
        delete []Temp3;
        delete []Temp2;
        delete []Temp1;
        // Making a lookup table.

        int m_iWidth = 2800;
        int m_iHeight = 600;

        SisoIoImageEngine* imageHandle0 = nullptr;
        const unsigned char* B1 = nullptr;
        auto error = IoImageOpen("C:/Users/minwoo/Desktop/Minu/CalTest/B1.tiff", &imageHandle0);
        B1 = (const unsigned char*)IoImageGetData(imageHandle0);

        const unsigned char* B2 = nullptr;
        error = IoImageOpen("C:/Users/minwoo/Desktop/Minu/CalTest/B2.tiff", &imageHandle0);
        B2 = (const unsigned char*)IoImageGetData(imageHandle0);

        const unsigned char* D1 = nullptr;
        error = IoImageOpen("C:/Users/minwoo/Desktop/Minu/CalTest/D1.tiff", &imageHandle0);
        D1 = (const unsigned char*)IoImageGetData(imageHandle0);

        const unsigned char* D2 = nullptr;
        error = IoImageOpen("C:/Users/minwoo/Desktop/Minu/CalTest/D2.tiff", &imageHandle0);
        D2 = (const unsigned char*)IoImageGetData(imageHandle0);
        // C:/Users/minwoo/Desktop/Minu/CalTest/B1.tiff
        // C:/Users/minwoo/Desktop/Minu/CalTest/B2.tiff
        // C:/Users/minwoo/Desktop/Minu/CalTest/D1.tiff
        // C:/Users/minwoo/Desktop/Minu/CalTest/D1.tiff

        std::ofstream lutfile;
        lutfile.open("C:/Users/minwoo/Desktop/Minu/CalTest/2800TT.txt");
        int cnt = 0;
        for (int h = 0; h < m_iHeight; h++) {
            for (int w = 0; w < m_iWidth; w++) {
                unsigned int d1_p = (unsigned int) D1[cnt];
                unsigned int d2_p = (unsigned int) D2[cnt];
                unsigned int b1_p = (unsigned int) B1[cnt];
                unsigned int b2_p = (unsigned int) B2[cnt];

                unsigned int pixel = d1_p + (d2_p << 8) + (b1_p << 16) + (b2_p << 24);
                lutfile << pixel << "\n";

                cnt += 1;
            }

            //after each line dummy pixels must be added
            //width/12 = ?    2800 / 12 = 233.333 -> 234? ??, 234 * 12= 2808 , 8 pixel ?? ??
            //width/12 = ?    2804 / 12 = 233.333 -> 234? ??, 234 * 12= 2808 , 8 pixel ?? ??
            if (m_iWidth % 12 != 0){
                for (int dummy = 0; dummy < 4; dummy++)
                    lutfile << 0 << "\n";
            }else{
                for (int dummy = 0; dummy < 4; dummy++)
                    lutfile << 0 << "\n";
            }
        }

        lutfile.close();
*/

        grabber->setCalibMode(true);
        QMessageBox::information(this, "Xiso", "Lookup table is done.");

    });
    // channel changes.
    connect(ui->comboBox_channel, &QComboBox::currentTextChanged, this ,[this](QString string){
        qDebug() << string << "mode is enabled.";
        if(string == "Grabber"){
            disconnect(detectorConnect);
            grabberConnect = connect(this->grabber, &Grabber::sendImage, this, [this](QImage image){
                this->widget->setImage(image);
                ui->statusbar->showMessage("Elapsed time : " + QString::number(timer->restart()));
            });
        }else{ // detector mode
            disconnect(grabberConnect);
            detectorConnect = connect(this->detector, &Detector::sendImage, this, [this](QImage image){
                this->widget->setImage(image);
                ui->statusbar->showMessage("Elapsed time : " + QString::number(timer->restart()));
            });
        }
    });
    // ROI set-up
    connect(ui->pushButton_roiApply, &QPushButton::clicked, this, [this](){
        this->setROI();
    });


}

MainWindow::~MainWindow()
{
    delete ui;
}


/// GRABBER PART BEGINS ///
void MainWindow::setGrabber(Grabber *c)
{
    grabber = c;
    ui->formLayout_grabber->addRow(grabber->getDialog());
    // Default Grabber
    //    grabberConnect = connect(this->grabber, &Grabber::sendImage, this, [this](QImage image){
    //        auto gen = bcControl->convertImage(&image);

    //        this->widget->setImage(gen);
    //        //        qDebug() << "try to save images" << image.save("C:/Users/User/Desktop/tmpMinu/test.tiff");
    //        ui->statusbar->showMessage("Elapsed time : " + QString::number(timer->restart()));
    //    });

}
/// DETECTOR PART BEGINS ///
void MainWindow::setDetector(Detector *d)
{
    detector = d;
    ui->formLayout_detector->addRow(detector->getDialog());
    connect(d, &Detector::sendBuffer, this->grabber, &Grabber::convertToGrabberImage);

    disconnect(grabberConnect);
    detectorConnect = connect(this->detector, &Detector::sendImage, this, [this](QImage image){
        this->widget->setImage(image);
        ui->statusbar->showMessage("Elapsed time : " + QString::number(timer->restart()));
    });
}

void MainWindow::setMessage(QString message)
{
    ui->textEdit->append(message);
}

void MainWindow::setROI()
{
    int x = ui->spinBox_roiX->value();
    int y = ui->spinBox_roiY->value();
    int width = ui->spinBox_width->value();
    int height = ui->spinBox_height->value();

    this->grabber->setXOffset(x);
    this->grabber->setYOffset(y);
    this->grabber->setImageWidth(width);
    this->grabber->setImageHeight(height);

    this->detector->setX(x);
    this->detector->setY(y);
    this->detector->setWidth(width);
    this->detector->setHeight(height);
}

