#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <fstream>

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
//    ui->formLayout->addRow(debug);

    instance = this;

    //    qInstallMessageHandler(setDebugMessage);
    connect(ui->actionPanel, &QAction::triggered, this, [this](bool on){this->ui->dockWidget_panel->setVisible(on);});
    connect(ui->dockWidget_panel, &QDockWidget::visibilityChanged, this, [this](bool on){this->ui->actionPanel->setChecked(on);});
    connect(ui->pushButton_Sequential, &QPushButton::clicked, this, [this](){
        emit grabbingStart(ui->spinBox_Sequential->value());
        ui->comboBox_exposureMode->setCurrentText("SEQ");
        timer->start();
    });
    connect(ui->pushButton_Continuous, &QPushButton::clicked, this, [this](bool on){
        if(on) emit grabbingStart();
        else emit grabbingFinished();
    });

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
        detector->setSaveMode(false);
    });
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
        detector->setSaveMode(false);
    });
    connect(ui->pushButton_out, &QPushButton::clicked, this, [this](){
        // Test Function now
        /*
        darkCalPath = "C:/Users/User/Desktop/Minu/Xiso/Test/Dark";
        brightCalPath = "C:/Users/User/Desktop/Minu/Xiso/Test/Bright";
        if(darkCalPath == "" || brightCalPath =
= ""){
            qDebug() << "Cal path is not set";
            return;
        }
        auto darkList = QDir(darkCalPath).entryList(QStringList() << "*.tif" << "*.tiff", QDir::Files);
        auto brightList = QDir(brightCalPath).entryList(QStringList() << "*.tif" << "*.tiff", QDir::Files);

        int cnt =0;
        QImage outPixel = QImage(2400, 600, QImage::Format_Grayscale16);
        outPixel.fill(0);

        foreach(const QString &imagePath, brightList){
            QString currentImagePath = brightCalPath + "/" + imagePath;
            qDebug() << currentImagePath;

            QImage currentImage(brightCalPath + "/" + imagePath);
            QImage testImage(brightCalPath +"/DMAOut.tiff");

            for(int y=0; y < currentImage.height(); ++y){
                for(int x=0; x < currentImage.width(); ++x){
                    QColor cColor= currentImage.pixelColor(x,y);
                    qreal corr = cColor.redF()/(qreal)10.;
                    qreal outPixelVal = outPixel.pixelColor(x,y).redF();
                    qreal out = outPixel.pixelColor(x,y).redF() + corr;
                    QColor nColor = QColor::fromRgbF(out,out,out, 1);

                    if(x==0 && y==0){
                        qDebug() << corr << out << nColor << outPixelVal;
                    }

                    outPixel.setPixelColor(x, y, nColor);
                }
            }
            outPixel.save(brightCalPath +"/" +QString::number(cnt++) +"___result.tiff");
        }
        */

        SisoIoImageEngine* imageHandle0 = NULL;
        int m_iWidth = this->ui->spinBox_width->value(); // Need to edit
        int m_iHeight = this->ui->spinBox_height->value(); // Need to edit
        int nByte = 2;
        int nAvgCount = ui->spinBox_calibCount->value();

        //        darkCalPath = "C:/Users/User/Desktop/Minu/Xiso/Test/Dark";
        //        brightCalPath = "C:/Users/User/Desktop/Minu/Xiso/Test/Bright";
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
        for (int n = 0; n < 2; n++){
            auto error = IoImageOpen((darkCalPath + "/DarkRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
            if(error == 0){
                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++){
                    for (int j = 0; j < m_iWidth; j++){
                        Temp1[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 2; n < 4; n++){
            auto error = IoImageOpen((darkCalPath + "/DarkRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
            if(error == 0){

                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++){
                    for (int j = 0; j < m_iWidth; j++){
                        Temp2[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 4; n < 6; n++){
            auto error = IoImageOpen((darkCalPath + "/DarkRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
            if(error == 0){

                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++){
                    for (int j = 0; j < m_iWidth; j++){
                        Temp3[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 6; n < 8; n++){
            auto error = IoImageOpen((darkCalPath + "/DarkRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
            if(error == 0){

                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++){
                    for (int j = 0; j < m_iWidth; j++){
                        Temp4[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 8; n < 10; n++){
            auto error = IoImageOpen((darkCalPath + "/DarkRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
            if(error == 0){

                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++){
                    for (int j = 0; j < m_iWidth; j++){
                        Temp5[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 10; n < 12; n++){
            auto error = IoImageOpen((darkCalPath + "/DarkRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
            if(error == 0){

                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++){
                    for (int j = 0; j < m_iWidth; j++){
                        Temp6[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 12; n < 14; n++){
            auto error = IoImageOpen((darkCalPath + "/DarkRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
            if(error == 0){

                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++){
                    for (int j = 0; j < m_iWidth; j++){
                        Temp7[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                    }
                }
            }
        }
        IoFreeImage(imageHandle0);
        imageHandle0 = NULL;
        for (int n = 14; n < 16; n++){
            auto error = IoImageOpen((darkCalPath + "/DarkRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
            if(error == 0){

                Temp = (unsigned short*)IoImageGetData(imageHandle0);

                for (int i = 0; i < m_iHeight; i++){
                    for (int j = 0; j < m_iWidth; j++){
                        Temp8[i * m_iWidth + j] += (Temp[i * m_iWidth + j]);
                    }
                }
            }
        }
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

        //        memset(Temp, 0, m_iWidth * m_iHeight * nByte);
        //        memset(Temp1, 0, m_iWidth * m_iHeight * nByte);
        //        memset(AveImg, 0, m_iWidth * m_iHeight * nByte);
        //        IoFreeImage(imageHandle0);
        //        imageHandle0 = NULL;

        int nSumImage = 0;
        int error = 0;

        for (int n = 0; n < 2; n++)
        {
            error = IoImageOpen((brightCalPath + "/WhiteRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
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
            error = IoImageOpen((brightCalPath + "/WhiteRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
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
            error = IoImageOpen((brightCalPath + "/WhiteRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
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
            error = IoImageOpen((brightCalPath + "/WhiteRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
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
            error = IoImageOpen((brightCalPath + "/WhiteRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
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
            error = IoImageOpen((brightCalPath + "/WhiteRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
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
            error = IoImageOpen((brightCalPath + "/WhiteRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
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
            error = IoImageOpen((brightCalPath + "/WhiteRaw" + QString::number(n) + ".tiff").toStdString().c_str(), &imageHandle0);
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
        IoWriteTiff((brightCalPath + "/WcalAvg.tiff").toStdString().c_str(), (unsigned char*)AveImg, m_iWidth, m_iHeight, 16, 1);
        //        IoWriteTiff("C:/Users/User/Desktop/Minu/Xiso/Test/Wcal/DcalAvg.tiff", (unsigned char*)AveImg, m_iWidth, m_iHeight, 16, 1);

        for (int i = 0; i < m_iHeight; i++)
        {
            for (int j = 0; j < m_iWidth; j++)
            {


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


        std::ofstream lutfile;
        lutfile.open("C:/5608/2800.txt");
        int cnt = 0;
        for (int h = 0; h < m_iHeight; h++) {
            for (int w = 0; w < m_iWidth; w++) {

                unsigned int d1_p = (unsigned int) cal_Data0[cnt];
                unsigned int d2_p = (unsigned int) cal_Data1[cnt];
                unsigned int b1_p = (unsigned int) cal_Data2[cnt];
                unsigned int b2_p = (unsigned int) cal_Data3[cnt];

                unsigned int pixel = d1_p + (d2_p << 8) + (b1_p << 16) + (b2_p << 24);
                lutfile << pixel << "\n";

                cnt += 1;
            }

            //after each line dummy pixels must be added
            //width/12 = ?    2800 / 12 = 233.333 -> 234? ??, 234 * 12= 2808 , 8 pixel ?? ??
            //width/12 = ?    2804 / 12 = 233.333 -> 234? ??, 234 * 12= 2808 , 8 pixel ?? ??
            if (m_iWidth % 12 != 0)
            {
                for (int dummy = 0; dummy < 8; dummy++)
                    lutfile << 0 << "\n";
            }
            else
            {
                for (int dummy = 0; dummy < 4; dummy++)
                    lutfile << 0 << "\n";

            }
        }

        lutfile.close();
        grabber->setCalibMode(true);
        QMessageBox::information(this, "Xiso", "Lookup table is done.");
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
    });
    connect(ui->pushButton_ResetGrabber, &QPushButton::clicked, this, [this](){
        this->grabber->stopGrabbing();
    });
}
/// DETECTOR PART BEGINS ///
void MainWindow::setDetector(Detector *d)
{
    detector = d;
    connect(ui->pushButton_DetectorInitialize, &QPushButton::clicked, this, [this](){
        detector->initialize();
        detector->setROI(ui->spinBox_width->value(), ui->spinBox_height->value(), 0, 0);
        detector->setExposureMode(SpectrumLogic::ExposureModes::fps25_mode);
        detector->setExposureTime(5000);
        ui->comboBox_BinningMode->setCurrentText("1X1");
        QMessageBox::information(this, "XViewer", "Detector is intialized.");
    });
    connect(ui->pushButton_roiApply, &QPushButton::clicked, this, [this](){
        detector->setROI(ui->spinBox_width->value(), ui->spinBox_height->value(),0,0);
        grabber->setROI(ui->spinBox_width->value(), ui->spinBox_height->value());
    });
    connect(ui->pushButton_detectorApply, &QPushButton::clicked, this, [this](){
        qDebug() << "Exposure Time : " << ui->spinBox_Exposure->value();
        qDebug() << "Expousre Mode : " << ui->comboBox_exposureMode->currentText();
        qDebug() << "Binning Mode : " << ui->comboBox_BinningMode->currentText();
        int expTime = ui->spinBox_Exposure->value();
        QString exMode = ui->comboBox_exposureMode->currentText();
        QString binning = ui->comboBox_BinningMode->currentText();
        bool error = false;
        if(binning == "1X1"){
            error = detector->setBinningMode(SpectrumLogic::BinningModes::x11);
        }else if(binning == "2X2"){
            error = detector->setBinningMode(SpectrumLogic::BinningModes::x22);
        }else if(binning == "4X4"){
            error = detector->setBinningMode(SpectrumLogic::BinningModes::x44);
        }else if(binning == "Unknown"){
            error = detector->setBinningMode(SpectrumLogic::BinningModes::BinningUnknown);
        }
        if(error) QMessageBox::information(this, "XViewer", "Binning mode is not applied.");

        if(exMode == "FPS25"){
            error = this->detector->setExposureMode(SpectrumLogic::ExposureModes::fps25_mode);
        }else if(exMode == "FPS30"){
            error = this->detector->setExposureMode(SpectrumLogic::ExposureModes::fps30_mode);
        }else if(exMode== "TRIG"){
            error = this->detector->setExposureMode(SpectrumLogic::ExposureModes::trig_mode);
        }else if(exMode == "XFPS"){
            error = this->detector->setExposureMode(SpectrumLogic::ExposureModes::xfps_mode);
        }else if(exMode == "SEQ"){
            error = this->detector->setExposureMode(SpectrumLogic::ExposureModes::seq_mode);
        }
        if(error) QMessageBox::information(this, "XViewer", "Exposure Mode is not applied.");

        error = this->detector->setExposureTime(expTime);
        if(error) QMessageBox::information(this, "XViewer", "Exposure time is not applied.");

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



