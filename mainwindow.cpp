#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

MainWindow *instance = nullptr;
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

    debug = new DebugConsole;
    ui->formLayout->addRow(debug);

    instance = this;

//    qInstallMessageHandler(setDebugMessage);
    connect(ui->actionPanel, &QAction::triggered, this, [this](bool on){this->ui->dockWidget_panel->setVisible(on);});
    connect(ui->dockWidget_panel, &QDockWidget::visibilityChanged, this, [this](bool on){this->ui->actionPanel->setChecked(on);});
    connect(ui->pushButton_Sequential, &QPushButton::clicked, this, [this](){
        emit grabbingStart(ui->spinBox_Sequential->value());
        timer->start();
    });
    connect(ui->pushButton_Continuous, &QPushButton::clicked, this, [this](bool on){
        if(on) emit grabbingStart();
        else emit grabbingFinished();
    });
    connect(ui->pushButton_ResetGrabber, &QPushButton::clicked, this, [this](){
        this->grabber->stopGrabbing();
    });
    connect(ui->pushButton_dark, &QPushButton::clicked, this, [this](){
        QString path = QFileDialog::getExistingDirectory(this, "Select a directory to save", QDir::homePath());
        if(path == "") return;
        darkCalPath = path;

        grabber->setCalibMode(false);
        // Grabbing x images
        detector->setSaveMode(true);
        detector->setSavingPath(path + "/");
        detector->sequentialGrabbing(ui->spinBox_calibCount->value());
        // need to capture X frames and save that frames in the specific folder.

        grabber->setCalibMode(true);
        detector->setSaveMode(false);

    });
    connect(ui->pushButton_bright, &QPushButton::clicked, this, [this](){
        QString path = QFileDialog::getExistingDirectory(this, "Select a directory to save", QDir::homePath());
        if(path == "") return;
        brightCalPath = path;

        grabber->setCalibMode(false);
        // Grabbing x images
        detector->setSaveMode(true);
        detector->setSavingPath(path + "/");
        detector->sequentialGrabbing(ui->spinBox_calibCount->value());

        grabber->setCalibMode(true);
        detector->setSaveMode(false);
    });
    connect(ui->pushButton_out, &QPushButton::clicked, this, [this](){
        // Test Function now
        darkCalPath = "C:/Users/User/Desktop/Minu/Xiso/Test/Dark";
        brightCalPath = "C:/Users/User/Desktop/Minu/Xiso/Test/Bright";
        if(darkCalPath == "" || brightCalPath == ""){
            qDebug() << "Cal path is not set";
            return;
        }
        auto darkList = QDir(darkCalPath).entryList(QStringList() << "*.tif" << "*.tiff", QDir::Files);
        auto brightList = QDir(brightCalPath).entryList(QStringList() << "*.tif" << "*.tiff", QDir::Files);

//        SisoIoImageEngine *imageHandler0 = nullptr;
//        int nByte = 2;
//        int nAvgCount;
//        unsigned short *tmp;

//        IoImageOpen((brightCalPath + "/" + brightList.first()).toStdString().c_str(), &imageHandler0);
//        QImage image((uchar*)IoImageGetData(imageHandler0), 2400, 600, QImage::Format_Grayscale8);

        QImage outPixel = QImage(2400, 600, QImage::Format_Grayscale16);
        outPixel.fill(0);
        foreach(const QString &name, brightList){
            QString currentImagePath = brightCalPath + "/" + name;
            QImage currentImage(currentImagePath);

            for(int y=0; y < currentImage.height(); ++y){
                for(int x=0; x < currentImage.width(); ++x){
                    outPixel.setPixel(x, y, outPixel.pixel(x,y) + (currentImage.pixel(x, y)/10));
                }
            }
        }

        outPixel.save(brightCalPath +"/" + "result.tiff");






        /*
         *
SisoIoImageEngine* imageHandle0 = NULL;

int nByte = 2;
int nAvgCount = m_iAvgImgCount;
unsigned short* Temp;// = new unsigned short[m_iWidth * m_iHeight * nByte];
//memset(Temp, 0, m_iWidth * m_iHeight * nByte);

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


    Dark Cal iamge Avergae ??
        for (int n = 0; n < nAvgCount; n++)
        {
            sprintf(ImageFile, "C:/5608/DCal/DarkRaw%d.tiff", n);
            IoImageOpen(ImageFile, &imageHandle0);

            Temp = (unsigned short*)IoImageGetData(imageHandle0);


            for (int i = 0; i < m_iHeight; i++)
            {
                for (int j = 0; j < m_iWidth; j++)
                {
                    Temp1[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);

                }
            }

        }
        for (int i = 0; i < m_iHeight; i++)
        {
            for (int j = 0; j < m_iWidth; j++)
            {
                AveImg[i * m_iWidth + j] = (Temp1[i * m_iWidth + j]) / nAvgCount;

            }
        }


        IoWriteTiff("C:/5608/DCal/DcalAvg.tiff", (unsigned char*)AveImg, m_iWidth, m_iHeight, 16, 1);

        for (int i = 0; i < m_iHeight; i++)
        {
            for (int j = 0; j < m_iWidth; j++)
            {


                // Low 8bit
                cal_Data0[i * m_iWidth + j] = AveImg[i * m_iWidth + j] / 1;   //BIT SHIFT 2^8=256 ->  (Bit7 ~ Bit0)
                // High 8bit
                cal_Data1[i * m_iWidth + j] = AveImg[i * m_iWidth + j] / 256;   //BIT SHIFT 2^8=256 ->  (Bit15 ~ Bit8)


            }
        }
        IoWriteTiff("C:/5608/DCal/D1.tiff", cal_Data0, m_iWidth, m_iHeight, 8, 1);
        IoWriteTiff("C:/5608/DCal/D2.tiff", cal_Data1, m_iWidth, m_iHeight, 8, 1);
        SetDlgItemText(IDC_STATIC, _T("Dcal Image Create Finish"));
        memset(Temp, 0, m_iWidth * m_iHeight * nByte);
        memset(Temp1, 0, m_iWidth * m_iHeight * nByte);
        memset(AveImg, 0, m_iWidth * m_iHeight * nByte);
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;

        int nSumImage = 0;
        int error = 0;

Gain Cal iamge Avergae ??

        for (int n = 0; n < 2; n++)
        {
            sprintf(ImageFile, "C:/5608/WCal/WhiteRaw%d.tiff", n);
            error = IoImageOpen(ImageFile, &imageHandle0);
            if (error == 0)
            {
                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++)
                {
                    for (int j = 0; j < m_iWidth; j++)
                    {
                        Temp1[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);

                    }
                }
            }

        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 2; n < 4; n++)
        {
            sprintf(ImageFile, "C:/5608/WCal/WhiteRaw%d.tiff", n);
            error = IoImageOpen(ImageFile, &imageHandle0);
            if (error == 0)
            {
                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++)
                {
                    for (int j = 0; j < m_iWidth; j++)
                    {
                        Temp2[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);

                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 4; n < 6; n++)
        {
            sprintf(ImageFile, "C:/5608/WCal/WhiteRaw%d.tiff", n);
            error = IoImageOpen(ImageFile, &imageHandle0);
            if (error == 0)
            {
                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++)
                {
                    for (int j = 0; j < m_iWidth; j++)
                    {
                        Temp3[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);

                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 6; n < 8; n++)
        {
            sprintf(ImageFile, "C:/5608/WCal/WhiteRaw%d.tiff", n);
            error = IoImageOpen(ImageFile, &imageHandle0);
            if (error == 0)
            {
                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++)
                {
                    for (int j = 0; j < m_iWidth; j++)
                    {
                        Temp4[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);

                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 8; n < 10; n++)
        {
            sprintf(ImageFile, "C:/5608/WCal/WhiteRaw%d.tiff", n);
            error = IoImageOpen(ImageFile, &imageHandle0);
            if (error == 0)
            {
                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++)
                {
                    for (int j = 0; j < m_iWidth; j++)
                    {
                        Temp5[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);

                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 10; n < 12; n++)
        {
            sprintf(ImageFile, "C:/5608/WCal/WhiteRaw%d.tiff", n);
            error = IoImageOpen(ImageFile, &imageHandle0);
            if (error == 0)
            {
                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++)
                {
                    for (int j = 0; j < m_iWidth; j++)
                    {
                        Temp6[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);

                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 12; n < 14; n++)
        {
            sprintf_s(ImageFile, "C:/5608/WCal/WhiteRaw%d.tiff", n);
            error = IoImageOpen(ImageFile, &imageHandle0);
            if (error == 0)
            {
                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++)
                {
                    for (int j = 0; j < m_iWidth; j++)
                    {
                        Temp7[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);

                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 14; n < 16; n++)
        {
            sprintf_s(ImageFile, "C:/5608/WCal/WhiteRaw%d.tiff", n);
            error = IoImageOpen(ImageFile, &imageHandle0);
            if (error == 0)
            {
                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++)
                {
                    for (int j = 0; j < m_iWidth; j++)
                    {
                        Temp8[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);

                    }
                }
            }
        }
        for (int i = 0; i < m_iHeight; i++)
        {
            for (int j = 0; j < m_iWidth; j++)
            {
                AveImg[i * m_iWidth + j] = ((Temp1[i * m_iWidth + j]) + (Temp2[i * m_iWidth + j]) + (Temp3[i * m_iWidth + j])+(Temp4[i * m_iWidth + j])+ (Temp5[i * m_iWidth + j])+ (Temp6[i * m_iWidth + j])+ (Temp7[i * m_iWidth + j])+ (Temp8[i * m_iWidth + j])) / nAvgCount;

            }
        }


        IoWriteTiff("C:/5608/WCal/WcalAvg.tiff", (unsigned char*)AveImg, m_iWidth, m_iHeight, 16, 1);

        for (int i = 0; i < m_iHeight; i++)
        {
            for (int j = 0; j < m_iWidth; j++)
            {


                // Low 8bit
                cal_Data0[i * m_iWidth + j] = AveImg[i * m_iWidth + j] / 1;   //BIT SHIFT 2^8=256 ->  (Bit7 ~ Bit0)
                // High 8bit
                cal_Data1[i * m_iWidth + j] = AveImg[i * m_iWidth + j] / 256;   //BIT SHIFT 2^8=256 ->  (Bit15 ~ Bit8)


            }
        }
        IoWriteTiff("C:/5608/WCal/B1.tiff", cal_Data0, m_iWidth, m_iHeight, 8, 1);
        IoWriteTiff("C:/5608/WCal/B2.tiff", cal_Data1, m_iWidth, m_iHeight, 8, 1);



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

        */
        // Making a lookup table.
        /*
         *
    datad1 = (const unsigned char*) IoImageGetData(d1);
    datad2 = (const unsigned char*) IoImageGetData(d2);
    datab1 = (const unsigned char*) IoImageGetData(b1);
    datab2 = (const unsigned char*) IoImageGetData(b2);

    //all heights must be the same otherwise it won't work!
    int h_d1 = IoGetHeight(d1);
    int w_d1 = IoGetWidth(d2);

    std::ofstream lutfile;
    lutfile.open("2800lut.txt");
    int cnt = 0;
    for (int h = 0; h < h_d1; h++) {
        for (int w = 0; w < w_d1; w++) {

            unsigned int d1_p = (unsigned int) datad1[cnt];
            unsigned int d2_p = (unsigned int) datad2[cnt];
            unsigned int b1_p = (unsigned int) datab1[cnt];
            unsigned int b2_p = (unsigned int) datab2[cnt];

            unsigned int pixel = d1_p + (d2_p << 8) + (b1_p << 16) + (b2_p << 24);
            lutfile << pixel << "\n";

            cnt += 1;
        }

        //after each line dummy pixels must be added
        //width/12 = ?    2800 / 12 = 233.333 -> 234? ??, 234 * 12= 2808 , 8 pixel ?? ??
        //width/12 = ?    2804 / 12 = 233.333 -> 234? ??, 234 * 12= 2808 , 8 pixel ?? ??
        if (2808 % 12 != 0)
        {
            int kkk = 0;
        }
        else
        {
            int kkk1 = 0;
        }
        for (int dummy = 0; dummy < 8; dummy++)
            lutfile << 0 << "\n";
    }

    lutfile.close();
*/
    });
    connect(ui->spinBox_Exposure, &QSpinBox::editingFinished, this, [=](){
//        qDebug() << "Exposure time edited" ;
//        this->detector->setExposureTime(ui->spinBox_Exposure->value());
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}


/// GRABBER PART BEGINS ///
void MainWindow::setGrabber(CGrabber *c)
{
    grabber = c;
    connect(grabber, &CGrabber::sendImage, this, [this](QImage image){
        this->widget->setImage(image);
        ui->statusbar->showMessage("Elapsed time : " + QString::number(timer->restart()));
    });
    connect(ui->pushButton_GrabberInitialize, &QPushButton::clicked, this, [this](){
        emit grabberInit();

        QMessageBox *box = new QMessageBox("Basler Framegrabber", "Try to initialize this grabber. please wait a while.", QMessageBox::Information,0,0,0,this);
        box->show();
        box->setText("Working is done. check the status and next step to run.");
    });
}
/// DETECTOR PART BEGINS ///
void MainWindow::setDetector(Detector *d)
{
    detector = d;
    connect(ui->pushButton_DetectorInitialize, &QPushButton::clicked, this, [this](){
        detector->initialize();
        detector->setROI(2400, 600, 0, 0);
        QMessageBox::information(this, "XViewer", "Detector is intialized.");
    });

    ui->comboBox_BinningMode->setCurrentText("1X1");
    connect(ui->comboBox_BinningMode, &QComboBox::currentTextChanged, this, [=](QString value){
        bool isSucced = false;
        if(value == "1X1"){
            qDebug() << "1X1 is selected.";
            isSucced = detector->setBinningMode(SpectrumLogic::BinningModes::x11);
        }else if(value == "2X2"){
            qDebug() << "2X2 is selected.";
            isSucced = detector->setBinningMode(SpectrumLogic::BinningModes::x22);
        }else if(value == "4X4"){
            qDebug() << "4X4 is selected.";
            isSucced = detector->setBinningMode(SpectrumLogic::BinningModes::x44);
        }else if(value == "Unknown"){
            qDebug() << "Unknown is selected.";
            isSucced = detector->setBinningMode(SpectrumLogic::BinningModes::BinningUnknown);
        }
        if(!isSucced) QMessageBox::information(this, "XViewer", "Failed to set a binning mode.");
    });
}

void MainWindow::setMessage(QString message)
{
//    ui->textEdit->append(message);
    debug->appendText(message);
}

void setDebugMessage(QtMsgType type, const QMessageLogContext &conText, const QString &msg)
{
    instance->setMessage(msg);
}



