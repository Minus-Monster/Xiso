#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <fstream>
#include <QTextBlock>
#include <QTextCursor>
#include <QMetaObject>
#include "sisoIo.h"
#include <QColorSpace>
#include <QDockWidget>

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

    bcControl = new BrightContrastControl(this);
    QImage *img = new QImage("C:/Users/MPark/Documents/Projects/Xiso/Dark/DarkRaw0.tiff");
    this->widget->setImage(*img);
    bcControl->convertImage(img);
    connect(bcControl, &BrightContrastControl::converted, this, [this](QImage image){
        this->widget->setImage(image);
    });
    connect(ui->pushButton_imageAdjustment, &QPushButton::clicked, this, [this](bool checked){
        if(checked) this->bcControl->show();
        else this->bcControl->hide();
    });
    connect(bcControl, &BrightContrastControl::finished, this, [this](bool on){
        this->ui->pushButton_imageAdjustment->setChecked(on);
    });

    /// DEBUG CONSOLE
    debugConsole = new QTextEdit;
    dockwidgetDebug = new QDockWidget("Debug Console");
    dockwidgetDebug->setWidget(debugConsole);
    addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea,dockwidgetDebug);
    connect(ui->actionDebug_Console, &QAction::triggered, dockwidgetDebug, &QDockWidget::setVisible);
    dockwidgetDebug->setGeometry(0,0, 100, 50);

    /// SHOWING DISPLAY CHANGING
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
    /// MAIN PANEL
    connect(ui->actionPanel, &QAction::triggered, ui->dockWidget_panel, &QDockWidget::setVisible);
    /// GRABBER AND DETECTOR SEQUENTIAL GRABBING
    connect(ui->pushButton_Sequential, &QPushButton::clicked, this, [this](){
        int cnt = ui->spinBox_Sequential->value();
        this->detector->setExposureMode(SpectrumLogic::ExposureModes::seq_mode);
        timer->start();
        emit grabbingStart(cnt);
        //                this->grabber->continuousGrabbing();
        //                QImage *img = new QImage("C:/Users/minwoo/Desktop/Minu/CalTest/Test_Raw.tiff");
        //                this->grabber->convertToGrabberImage((unsigned short*)img->bits());
    });
    /// GRABBER AND DETECTOR CONTINUOUS GRABBING
    connect(ui->pushButton_Continuous, &QPushButton::clicked, this, [this](bool on){
        if(on) emit grabbingStart();
        else emit grabbingFinished();
        timer->start();
    });
    /// GRABBER AND DETECTOR STOP GRABBING
    connect(ui->pushButton_reset, &QPushButton::clicked, this, [this](){
        emit grabbingFinished();
        timer->elapsed();
    });
    /// DARK CALIBRATION MODE
    connect(ui->pushButton_dark, &QPushButton::clicked, this, [this](){
        QString path = QFileDialog::getExistingDirectory(this, "Select a directory to save", QDir::homePath());
        if(path.isEmpty()) return;
        darkCalPath = path;
        // Grabbing x images
        detector->setSaveMode(true);
        detector->setSavingPath(path + "/DarkRaw");
        detector->sequentialGrabbing(ui->spinBox_calibCount->value());
        /// I'm not sure if it is fixed that the detector doesn't stop capturing after this function;
        /// When executing another calib(eg,. bright cal), you may need to push the stop grabbing button first (It is a known bug)
        detector->setSaveMode(false); // need to revise this code. threading error.
        //        detector->saveImages(ui->spinBox_calibCount->value());
        // need to capture X frames and save that frames in the specific folder.
    });
    /// BRIGHT CALIBRATION MODE
    connect(ui->pushButton_bright, &QPushButton::clicked, this, [this](){
        QString path = QFileDialog::getExistingDirectory(this, "Select a directory to save", QDir::homePath());
        if(path == "") return;
        brightCalPath = path;

        // Grabbing x images
        detector->setSaveMode(true);
        detector->setSavingPath(path + "/WhiteRaw");
        detector->sequentialGrabbing(ui->spinBox_calibCount->value());
        detector->setSaveMode(false);

    });
    /// AVERAGING DARK AND BRIGHT CALIBRATION
    connect(ui->pushButton_out, &QPushButton::clicked, this, [this](){
        //        darkCalPath = "C:/Users/MPark/Documents/Projects/Xiso/Dark";
        //        brightCalPath = "C:/Users/MPark/Documents/Projects/Xiso/Bright";

        if(darkCalPath == "" || brightCalPath ==  ""){
            qDebug() << "Cal path is not set";
            return;
        }
        auto darkList = QDir(darkCalPath).entryList(QStringList() << "*.tif" << "*.tiff", QDir::Files);
        foreach(auto current, darkList){
            if(!current.contains("DarkRaw")){
                darkList.removeOne(current);
            }
        }
        auto brightList = QDir(brightCalPath).entryList(QStringList() << "*.tif" << "*.tiff", QDir::Files);
        foreach(auto current, brightList){
            if(!current.contains("WhiteRaw")){
                brightList.removeOne(current);
            }
        }

        QString darkFileName = darkCalPath+"/DcalAvg";
        if(!this->averagedImage(darkFileName, darkCalPath, darkList)){
            qDebug() << "Dark calibration isn't generated.";
            return;
        }
        QString brightFileName = brightCalPath+"/WcalAvg";
        if(!this->averagedImage(brightFileName, brightCalPath, brightList)){
            qDebug() << "Bright calibration isn't generated.";
            return;
        }
        if(!this->generateLUT(darkFileName, brightFileName, darkCalPath+"/2800lut.txt")){
            qDebug() << "LUT Generating has faced an error.";
            return;
        }
        QMessageBox::information(this, "Xiso", "Lookup table is done.");

    });
    /// ROI set-up
    connect(ui->pushButton_roiApply, &QPushButton::clicked, this, [this](){
        this->setROI();
    });
}

MainWindow::~MainWindow(){
    delete ui;
}


/// GRABBER PART BEGINS ///
void MainWindow::setGrabber(Grabber *c)
{
    grabber = c;

    dockwidgetGrabber = new QDockWidget("Grabber Configuration");
    dockwidgetGrabber->setWidget(grabber->getDialog());
    this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dockwidgetGrabber);
    dockwidgetGrabber->setVisible(false);

    connect(ui->actionGrabber_Settings, &QAction::triggered, dockwidgetGrabber, &QDockWidget::setVisible);
    connect(dockwidgetGrabber, &QDockWidget::visibilityChanged, ui->actionGrabber_Settings, &QAction::setChecked);
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
    connect(detector, &Detector::sendBuffer, this->grabber, &Grabber::convertToGrabberImage);

    dockwidgetDetector = new QDockWidget("Detector Configuration");
    dockwidgetDetector->setWidget(detector->getDialog());
    this->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockwidgetDetector);
    dockwidgetDetector->setVisible(false);
    connect(ui->actionDetector_Settings, &QAction::triggered, dockwidgetDetector, &QDockWidget::setVisible);
    connect(dockwidgetDetector, &QDockWidget::visibilityChanged, ui->actionDetector_Settings, &QAction::setChecked);

    disconnect(grabberConnect);
    detectorConnect = connect(this->detector, &Detector::sendImage, this, [this](QImage image){
        this->widget->setImage(image);
        ui->statusbar->showMessage("Elapsed time : " + QString::number(timer->restart()));
    });
}

void MainWindow::setMessage(QString message)
{
    debugConsole->append(message);
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

    qDebug() << (this->detector->setROI(width, height,x,y) ? "Completely set values of the detector": "Failed to set values of the detector");
}

bool MainWindow::averagedImage(QString avgFileName, QString dirPath, QStringList fileNameList)
{
    if(fileNameList.isEmpty()){
        qDebug() << dirPath << "the file list is empty. nothing can do.";
        return false;
    }
    QVector<unsigned short*> bufs;
    SisoIoImageEngine* imageHandler = nullptr;
    foreach(const QString &currentFile, fileNameList){
        QString currentImagePath = dirPath + "/" + currentFile;
        qDebug() << "Appended" << currentImagePath;

        if(IoImageOpen(currentImagePath.toStdString().c_str(), &imageHandler) != 0){
            qDebug() << "Failed to open a file via IoImageOpen. this sequence will be closed.";
            return false;
        }
        bufs.push_back((unsigned short*)IoImageGetData(imageHandler));
    }
    QImage tmp = QImage(dirPath + "/" + fileNameList.first());
    if(tmp.isNull()){
        qDebug() << "File is empty." << avgFileName << "Wouldn't be made.";
        return false;
    }
    int bytePerPixel = 2; // 16 bit -> 2 , 8 bit -> 1

    unsigned short* averageBuf = (unsigned short*)malloc(tmp.width()*tmp.height()*bytePerPixel);
    unsigned char* upperbits = (unsigned char*)malloc(tmp.width()*tmp.height());
    unsigned char* lowerbits = (unsigned char*)malloc(tmp.width()*tmp.height());
    for(int y=0; y < tmp.height(); ++y){
        for(int x=0; x < tmp.width(); ++x){
            unsigned long sumVal = 0;
            unsigned int currentLineNum = y * tmp.width() + x;
            for(unsigned short* buf : bufs){
                sumVal += buf[currentLineNum];
            }
            sumVal /= bufs.size();
            averageBuf[currentLineNum] = sumVal;
            upperbits[currentLineNum] = sumVal / 256;
            lowerbits[currentLineNum] = sumVal / 1;
        }
    }
    int error = IoWriteTiff((avgFileName + ".tiff").toStdString().c_str(), (unsigned char*)averageBuf, tmp.width(), tmp.height(), 16, 1);
    int errorUp = IoWriteTiff((avgFileName + "_upper.tiff").toStdString().c_str(), (unsigned char*)upperbits, tmp.width(), tmp.height(), 8, 1);
    int errorLow = IoWriteTiff((avgFileName.toStdString() + "_lower.tiff").c_str(), (unsigned char*)lowerbits, tmp.width(), tmp.height(), 8, 1);

    // Deallocating
    IoFreeImage(imageHandler);
    free(averageBuf);

    if(error || errorUp || errorLow){
        qDebug() << "TIFF Writing error: average-" << Fg_getErrorDescription(this->grabber->getFg(), error) << "Upper-" << Fg_getErrorDescription(this->grabber->getFg(), errorUp) << "Lower-" << Fg_getErrorDescription(this->grabber->getFg(), errorLow);
        return false;
    }
    return true;
}

bool MainWindow::generateLUT(QString darkAvgFileName, QString brightAvgFileName, QString savePath)
{
    SisoIoImageEngine* imageHandler = nullptr;
    unsigned char *B1, *B2, *D1, *D2;
    int errorB1, errorB2, errorD1, errorD2;

    errorD1 = IoImageOpen((darkAvgFileName + "_lower.tiff").toStdString().c_str(), &imageHandler);
    D1 = (unsigned char*)IoImageGetData(imageHandler);
    errorD2 = IoImageOpen((darkAvgFileName + "_upper.tiff").toStdString().c_str(), &imageHandler);
    D2 = (unsigned char*)IoImageGetData(imageHandler);
    errorB1 = IoImageOpen((brightAvgFileName + "_lower.tiff").toStdString().c_str(), &imageHandler);
    B1 = (unsigned char*)IoImageGetData(imageHandler);
    errorB2 = IoImageOpen((brightAvgFileName + "_upper.tiff").toStdString().c_str(), &imageHandler);
    B2 = (unsigned char*)IoImageGetData(imageHandler);

    if(errorD1 || errorD2 || errorB1 || errorB2){
        qDebug() << "Cannot open some files from the path. failed to create LUT.";
        return false;
    }

    std::ofstream lutfile;
    lutfile.open(savePath.toStdString().c_str());
    QImage tmp = QImage(darkAvgFileName + "_upper.tiff");

    int cnt = 0;
    for (int h = 0; h < tmp.height(); h++) {
        for (int w = 0; w < tmp.width(); w++) {
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
        if (tmp.width() % 12 != 0){
            for (int dummy = 0; dummy < 4; dummy++)
                lutfile << 0 << "\n";
        }else{
            for (int dummy = 0; dummy < 4; dummy++)
                lutfile << 0 << "\n";
        }
    }
    lutfile.close();
    return true;
}

