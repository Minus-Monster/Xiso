#include "DetectorDialog.h"
#include "ui_DetectorDialog.h"
#include "Detector.h"
#include <qmessagebox.h>

DetectorDialog::DetectorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DetectorDialog)
{
    ui->setupUi(this);
    setWindowTitle("Detector Settings");
    connect(ui->pushButton_init, &QPushButton::clicked, this, [this](){
        // Init detector
        bool enable =this->detector->initialize();
        if(!enable) {
            QMessageBox::critical(this, this->windowTitle(), "Detector initializing is failed." );
            return;
        }
        this->ui->groupBox_settings->setEnabled(enable);

        // Exp time
        this->ui->spinBox_exposureTime->setValue(this->detector->getExposureTime());
        // Exp mode
        auto exMode = detector->getExposureMode();
        if(exMode == SpectrumLogic::ExposureModes::fps25_mode){
            ui->comboBox_exposureMode->setCurrentText("FPS25");
        }else if(exMode == SpectrumLogic::ExposureModes::fps30_mode){
            ui->comboBox_exposureMode->setCurrentText("FPS30");
        }else if(exMode == SpectrumLogic::ExposureModes::seq_mode){
            ui->comboBox_exposureMode->setCurrentText("SEQ");
        }else if(exMode == SpectrumLogic::ExposureModes::trig_mode){
            ui->comboBox_exposureMode->setCurrentText("TRIG");
        }else if(exMode == SpectrumLogic::ExposureModes::xfps_mode){
            ui->comboBox_exposureMode->setCurrentText("XFPS");
        }
        // Binning
        auto binning = detector->getBinningMode();
        switch(SpectrumLogic::BinningModes::BinningUnknown){
        case SpectrumLogic::BinningModes::x11:{
            ui->comboBox_binningMode->setCurrentText("1X1");
            break;
        }case SpectrumLogic::BinningModes::x22:{
            ui->comboBox_binningMode->setCurrentText("2X2");
            break;
        }case SpectrumLogic::BinningModes::x44:{
            ui->comboBox_binningMode->setCurrentText("4X4");
            break;
        }case SpectrumLogic::BinningModes::BinningUnknown:
        default:{
            ui->comboBox_binningMode->setCurrentText("Unknown");
            qDebug() << "binning mode has a werid parameter. tracking number is" << (int)binning;
        }}
        // ROI
        this->ui->spinBox_width->setValue(this->detector->getWidth());
        this->ui->spinBox_height->setValue(this->detector->getHeight());
        this->ui->spinBox_x->setValue(this->detector->getX());
        this->ui->spinBox_y->setValue(this->detector->getY());

        QMessageBox::information(this, this->windowTitle(), "Detector is ready to use.");
    });
    // Exp Time
    connect(ui->spinBox_exposureTime, &QSpinBox::editingFinished, this, [=](){
        ui->spinBox_exposureTime->blockSignals(true);
        this->detector->setExposureTime(this->ui->spinBox_exposureTime->value());
        ui->spinBox_exposureTime->blockSignals(false);
    });
    // Exp Mode
    connect(ui->comboBox_exposureMode, &QComboBox::currentTextChanged, this, [=](QString value){
        auto currentMode = ui->comboBox_exposureMode->currentText();
        if(currentMode == "FPS25") this->detector->setExposureMode(SpectrumLogic::ExposureModes::fps25_mode);
        else if(currentMode == "FPS30") this->detector->setExposureMode(SpectrumLogic::ExposureModes::fps30_mode);
        else if(currentMode == "SEQ") this->detector->setExposureMode(SpectrumLogic::ExposureModes::seq_mode);
        else if(currentMode == "TRIG") this->detector->setExposureMode(SpectrumLogic::ExposureModes::trig_mode);
        else if(currentMode == "XFPS") this->detector->setExposureMode(SpectrumLogic::ExposureModes::xfps_mode);
        else;
    });
    // Bin Mode
    connect(ui->comboBox_binningMode, &QComboBox::currentTextChanged, this, [=](QString value){
        auto currentBinning = ui->comboBox_binningMode->currentText();
        if(currentBinning == "1X1") this->detector->setBinningMode(SpectrumLogic::BinningModes::x11);
        else if(currentBinning == "2X2") this->detector->setBinningMode(SpectrumLogic::BinningModes::x22);
        else if(currentBinning == "4X4") this->detector->setBinningMode(SpectrumLogic::BinningModes::x44);
        else if(currentBinning == "Unknown") this->detector->setBinningMode(SpectrumLogic::BinningModes::BinningUnknown);
        else;
    });
    // FullWell
    connect(ui->comboBox_fullwell, &QComboBox::currentTextChanged, this, [=](QString value){
        // Not implementation because it doesn't support to get this value.
        auto currentFull = ui->comboBox_fullwell->currentText();
        if(currentFull == "Low") this->detector->setFullWell(SpectrumLogic::FullWellModes::Low);
        if(currentFull == "High") this->detector->setFullWell(SpectrumLogic::FullWellModes::High);
        if(currentFull == "Unknown") this->detector->setFullWell(SpectrumLogic::FullWellModes::Unknown);
    });
    // ROI
    connect(ui->spinBox_width, &QSpinBox::editingFinished, this, [=](){
        this->detector->setWidth(this->ui->spinBox_width->value());
    });
    connect(ui->spinBox_height, &QSpinBox::editingFinished, this, [=](){
        this->detector->setHeight(this->ui->spinBox_height->value());
    });
    connect(ui->spinBox_x, &QSpinBox::editingFinished, this, [=](){
        this->detector->setX(this->ui->spinBox_x->value());
    });
    connect(ui->spinBox_y, &QSpinBox::editingFinished, this, [=](){
        this->detector->setY(this->ui->spinBox_y->value());
    });

    // Sequential Grabbing
    connect(ui->pushButton_sequential, &QPushButton::clicked, this, [this](){
        this->detector->sequentialGrabbing(ui->spinBox_sequenceCnt->value());
    });
    // Continuous Grabbing
    connect(ui->pushButton_continuous, &QPushButton::clicked, this, [this](){
        this->detector->continuousGrabbing();
    });
    // Stop
    connect(ui->pushButton_stop, &QPushButton::clicked, this, [this](){
        this->detector->stopGrabbing();
    });

}

DetectorDialog::~DetectorDialog()
{
    delete ui;
}

void DetectorDialog::updateInformation()
{
    ui->spinBox_exposureTime->setValue(this->detector->getExposureTime());
    //    ui->comboBox_exposureMode
    auto exMode = this->detector->getExposureMode();
    switch(exMode){
    case SpectrumLogic::ExposureModes::fps25_mode:{
        ui->comboBox_exposureMode->setCurrentText("FPS25");
        break;
    }case SpectrumLogic::ExposureModes::fps30_mode:{
        ui->comboBox_exposureMode->setCurrentText("FPS30");
        break;
    }case SpectrumLogic::ExposureModes::trig_mode:{
        ui->comboBox_exposureMode->setCurrentText("TRIG");
        break;
    }case SpectrumLogic::ExposureModes::seq_mode:{
        ui->comboBox_exposureMode->setCurrentText("SEQ");
        break;
    }case SpectrumLogic::ExposureModes::xfps_mode:{
        ui->comboBox_exposureMode->setCurrentText("XFPS");
        break;
    }}
    auto binning = this->detector->getBinningMode();
    switch(binning){
    case SpectrumLogic::BinningModes::x11:{
        ui->comboBox_binningMode->setCurrentText("1X1");
        break;
    }case SpectrumLogic::BinningModes::x22:{
        ui->comboBox_binningMode->setCurrentText("2X2");
        break;
    }case SpectrumLogic::BinningModes::x44:{
        ui->comboBox_binningMode->setCurrentText("4X4");
        break;
    }case SpectrumLogic::BinningModes::BinningUnknown:{
        ui->comboBox_binningMode->setCurrentText("Unknown");
    }default:{

    }}
    auto fullwell = this->detector->getFullWellMode();
    switch(fullwell){
    case SpectrumLogic::FullWellModes::High:{
        ui->comboBox_fullwell->setCurrentText("High");
        break;
    }case SpectrumLogic::FullWellModes::Low:{
        ui->comboBox_fullwell->setCurrentText("Low");
        break;
    }case SpectrumLogic::FullWellModes::Unknown:{
        ui->comboBox_fullwell->setCurrentText("Unknown");
        break;
    }
    }

    ui->spinBox_width->setValue(this->detector->getWidth());
    ui->spinBox_height->setValue(this->detector->getHeight());
    ui->spinBox_x->setValue(this->detector->getX());
    ui->spinBox_y->setValue(this->detector->getY());
}

