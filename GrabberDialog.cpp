#include "GrabberDialog.h"
#include "ui_GrabberDialog.h"
#include "Grabber.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

GrabberDialog::GrabberDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GrabberDialog)
{
    ui->setupUi(this);
    setWindowTitle("Basler Framegrabber Settings");

    connect(ui->pushButton_loadApplet, &QPushButton::clicked, this, [this](){
        auto applet = QFileDialog::getOpenFileName(this, "Basler Framegrabber Settings", QDir::currentPath(), "*.hap *.dll");
        if(applet.isEmpty()) return;
        else ui->lineEdit_applet->setText(applet);
    });
    connect(ui->pushButton_loadConfig, &QPushButton::clicked, this, [this](){
        auto config = QFileDialog::getOpenFileName(this, "Basler Framegrabber Settings", QDir::currentPath(), "*.mcf");
        if(config.isEmpty()) return;
        else ui->lineEdit_config->setText(config);
        widgetGenerator(config);
    });

    connect(ui->pushButton_initialization, &QPushButton::clicked, this, [this](){
        auto applet = ui->lineEdit_applet->text();
        if(applet.isEmpty()) QMessageBox::critical(this, windowTitle(), "You need to set an applet path. Please check that path first.");
        auto config = ui->lineEdit_config->text();
        if(applet.isEmpty()) QMessageBox::critical(this, windowTitle(), "You need to set a config path. Please check that path first.");


        if(!grabber->loadApplet(applet)){
            QMessageBox::warning(this, this->windowTitle(), "An applet file is not loaded. ");
            return;
        }
        if(!grabber->loadConfiguration(config)){
            //            QMessageBox::warning(this, this->windowTitle(), "A config file is not loaded. ");
            //            return;
        }
        grabber->initialize();
        ui->groupBox_roi->setEnabled(true);
        ui->groupBox_userConfig->setEnabled(true);

        // Get parameters.
        this->updateInformation();

        // Width
        connect(ui->spinBox_width, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int){
            ui->spinBox_width->setKeyboardTracking(false);
            ui->spinBox_width->blockSignals(true);
            if(!this->grabber->setImageWidth(ui->spinBox_width->value())){
                ui->spinBox_width->setValue(this->grabber->getImageWidth());
                QMessageBox::warning(this, this->windowTitle(), "Couldn't apply new value to width parameter.");
            }else qDebug() << "Width value is applied correctly.";
            ui->spinBox_width->blockSignals(false);
        });
        // Height
        connect(ui->spinBox_height, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int){
            ui->spinBox_height->setKeyboardTracking(false);
            ui->spinBox_height->blockSignals(true);
            if(!this->grabber->setImageHeight(ui->spinBox_height->value())){
                ui->spinBox_height->setValue(this->grabber->getImageHeight());
                QMessageBox::warning(this, this->windowTitle(), "Couldn't apply new value to height parameter.");
            }else qDebug() << "Height value is applied correctly.";
            ui->spinBox_height->blockSignals(false);
        });
        // X
        connect(ui->spinBox_x, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int){
            ui->spinBox_x->setKeyboardTracking(false);
            ui->spinBox_x->blockSignals(true);
            if(!this->grabber->setXOffset(ui->spinBox_x->value())){
                ui->spinBox_x->setValue(this->grabber->getXOffset());
                QMessageBox::warning(this, this->windowTitle(), "Couldn't apply new value to x parameter.");
            }else qDebug() << "X value is applied correctly.";
            ui->spinBox_x->blockSignals(false);
        });
        // Y
        connect(ui->spinBox_y, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int){
            ui->spinBox_y->setKeyboardTracking(false);
            ui->spinBox_y->blockSignals(true);
            if(!this->grabber->setYOffset(ui->spinBox_y->value())){
                ui->spinBox_y->setValue(this->grabber->getYOffset());
                QMessageBox::warning(this, this->windowTitle(), "Couldn't apply new value to y parameter.");
            }else qDebug() << "Y value is applied correctly.";
            ui->spinBox_y->blockSignals(false);
        });
        connect(ui->checkBox_SCEnable, &QCheckBox::stateChanged, this, [this](){
            ui->checkBox_SCEnable->blockSignals(true);
            if(!this->grabber->setShadingCorrectionEnable(ui->checkBox_SCEnable->isChecked())){
                ui->checkBox_SCEnable->setChecked(this->grabber->getShadingCorrectionEnable());
                QMessageBox::warning(this, this->windowTitle(), "Couldn't apply new value to SC Enable parameter.");
            }else qDebug() << "SC Enable value is applied correctly.";
            ui->checkBox_SCEnable->blockSignals(false);
        });
        connect(ui->checkBox_loadInitFile, &QCheckBox::stateChanged, this, [this](){
            ui->checkBox_loadInitFile->blockSignals(true);
            if(!this->grabber->setInitFile(ui->checkBox_loadInitFile->isChecked())){
                ui->checkBox_loadInitFile->setChecked(this->grabber->getInitFile());
                QMessageBox::warning(this, this->windowTitle(), "Couldn't apply new value to Load init file parameter.\nMay you can ignore this error.");
            }else qDebug() << "Load Init file value is applied correctly.";
            ui->checkBox_loadInitFile->blockSignals(false);
        });
        connect(ui->spinBox_SCOverSaturation, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int){
            ui->spinBox_SCOverSaturation->setKeyboardTracking(false);
            ui->spinBox_SCOverSaturation->blockSignals(true);
            if(!this->grabber->setOverSaturation(ui->spinBox_SCOverSaturation->value())){
                ui->spinBox_SCOverSaturation->setValue(this->grabber->getOverSaturation());
                QMessageBox::warning(this, this->windowTitle(), "Couldn't apply new value to Over saturation parameter.");
            }else qDebug() << "Over saturation value is applied correctly.";
            ui->spinBox_SCOverSaturation->blockSignals(false);
        });
        connect(ui->lineEdit_LUTFilePath, &QLineEdit::editingFinished, this, [this](){
            ui->lineEdit_LUTFilePath->blockSignals(true);
            auto editied = ui->lineEdit_LUTFilePath->text().replace(QString("\\"), QString("/"));

            if(!this->grabber->setLUTFileName(editied)){
                ui->lineEdit_LUTFilePath->setText(this->grabber->getLUTFileName());
                QMessageBox::warning(this, this->windowTitle(), "Couldn't apply new value to LUT file path parameter.");
            }else{
                qDebug() << "LUT File path is applied.";
                ui->lineEdit_LUTFilePath->setText(editied);
            }
            ui->lineEdit_LUTFilePath->blockSignals(false);
        });
        connect(ui->checkBox_deadPixelInterpolation, &QCheckBox::stateChanged, this, [this](){
            ui->checkBox_deadPixelInterpolation->blockSignals(true);
            if(!this->grabber->setDeadPixelInterpolation(ui->checkBox_deadPixelInterpolation->isChecked())){
                ui->checkBox_deadPixelInterpolation->setChecked(this->grabber->getDeadPixelInterpolation());
                QMessageBox::warning(this, this->windowTitle(), "Couldn't apply new value to Deadpixel interpolation parameter.");
            }else qDebug() << "Deadpixel interpolation value is applied correctly.";
            ui->checkBox_deadPixelInterpolation->blockSignals(false);
        });
        connect(ui->checkBox_removeDarkH, &QCheckBox::stateChanged, this, [this](){
            ui->checkBox_removeDarkH->blockSignals(true);
            if(!this->grabber->setRemovingDarkH(ui->checkBox_removeDarkH->isChecked())){
                ui->checkBox_removeDarkH->setChecked(this->grabber->getRemovingDarkH());
                QMessageBox::warning(this, this->windowTitle(), "Couldn't apply new value to Removing dark H parameter.");
            }else qDebug() << "Removing dark H value is applied correctly.";
            ui->checkBox_removeDarkH->blockSignals(false);
        });
        connect(ui->checkBox_removeDarkV, &QCheckBox::stateChanged, this, [this](){
            ui->checkBox_removeDarkV->blockSignals(true);
            if(!this->grabber->setRemovingDarkV(ui->checkBox_removeDarkV->isChecked())){
                ui->checkBox_removeDarkV->setChecked(this->grabber->getRemovingDarkV());
                QMessageBox::warning(this, this->windowTitle(), "Couldn't apply new value to Removing dark V parameter.");
            }else qDebug() << "Removing dark V value is applied correctly.";
            ui->checkBox_removeDarkV->blockSignals(false);
        });
        connect(ui->pushButton_continuous, &QPushButton::clicked, this, [this](){
            this->grabber->continuousGrabbing();
        });
        connect(ui->pushButton_stop, &QPushButton::clicked, this, [this](){
            this->grabber->stopGrabbing();
        });
        connect(ui->pushButton_testPath, &QPushButton::clicked, this, [this](){
            auto path = QFileDialog::getOpenFileName(this, this->windowTitle(), QDir::currentPath(), "*.jpg *.png *.tif *.tiff");
            if(path.isEmpty()){
                ui->pushButton_testPath->setChecked(false);
                return;
            }else{
                currentTestFilePath = path;
                qDebug() << currentTestFilePath;
            }
        });
        connect(ui->pushButton_testShot, &QPushButton::clicked, this, [this](){
            QImage img(currentTestFilePath);
            this->grabber->convertToGrabberImage((unsigned short*)img.bits());

            //            SisoIoImageEngine *imageHandler0 = nullptr;
            //            IoImageOpen(currentTestFilePath.toStdString().c_str(), &imageHandler0);
            //            unsigned short* buf = (unsigned short*)malloc((size_t)grabber->getDMALength());
            //            memcpy(buf, (unsigned short*)IoImageGetData(imageHandler0),grabber->getDMALength());
            //            this->grabber->convertToGrabberImage(buf);

            qDebug() << "Test image is sent to this grabber.";
        });
        connect(ui->pushButton_saveMcf, &QPushButton::clicked, this, [this](){
            QString path = QFileDialog::getSaveFileName(this, this->windowTitle(), QDir::currentPath(), ".mcf");
            if(path.isEmpty()) return;

            this->grabber->saveConfig(path);
            qDebug() << "Save config called. ";
        });
        QMessageBox::information(this, this->windowTitle(), "Initialization is done.");
    });

    widget = new QTreeWidget;
    widget->show();
    //    widget->setColumnCount(2);
    widget->setHeaderLabels(QStringList() << "Feature" << "Value");
}

GrabberDialog::~GrabberDialog()
{
    delete ui;
}

void GrabberDialog::setGrabber(Grabber *_grabber)
{
    grabber = _grabber;
}

void GrabberDialog::widgetGenerator(QString mcfPath)
{
    QFile file(mcfPath);

    QString sectionParser = "[";
    QString valueParser = "=";

    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        QTreeWidgetItem *currentParent=nullptr;
        while(!in.atEnd()){
            QString line = in.readLine();
            if(line.isEmpty()) continue;

            if(line.contains(sectionParser)){
                QTreeWidgetItem *section = new QTreeWidgetItem(widget);
                section->setText(0, line.remove("[").remove("]"));
                currentParent = section;
                continue;
            }
            auto values = line.split(valueParser);

            QTreeWidgetItem *children = new QTreeWidgetItem(currentParent);
            children->setText(0, values.first());

            QLineEdit *lineEdit = new QLineEdit(values.last().remove(";"));
            lineEdit->setFrame(false);
            lineEdit->setObjectName(values.first());

            widget->setItemWidget(children, 1, lineEdit);
            connect(lineEdit, &QLineEdit::editingFinished, this, [=]{
                qDebug() << lineEdit->objectName();
                qDebug() << this->widget->selectedItems().first()->parent()->text(0);
                qDebug() << this->widget->selectedItems().first()->text(0);
//                qDebug() << this->widget->findChild<QTreeWidgetmItem *>(lineEdit->objectName(), Qt::FindChildrenRecursively);

//                for(auto edit : this->widget->findChildren<QTreeWidgetItem*>(lineEdit->objectName(), Qt::FindChildrenRecursively)){
//                    qDebug() << "?"<< edit;
//                }
            });

            qDebug() << line;
        }
    }
}

void GrabberDialog::generateChildrenWidgetItem(QTreeWidgetItem *parent, QString children)
{

}

void GrabberDialog::updateInformation()
{

    // X, Y, WIDTH, HEIGHT
    ui->spinBox_width->setValue(grabber->getImageWidth());
    ui->spinBox_height->setValue(grabber->getImageHeight());
    ui->spinBox_x->setValue(grabber->getXOffset());
    ui->spinBox_y->setValue(grabber->getYOffset());
    // OPTIONS
    ui->checkBox_SCEnable->setChecked(grabber->getShadingCorrectionEnable());
    ui->checkBox_loadInitFile->setChecked(grabber->getInitFile());
    ui->lineEdit_LUTFilePath->setText(grabber->getLUTFileName());
    ui->spinBox_SCOverSaturation->setValue(grabber->getOverSaturation());

    ui->checkBox_deadPixelInterpolation->setChecked(grabber->getDeadPixelInterpolation());
    ui->checkBox_removeDarkV->setChecked(grabber->getRemovingDarkV());
    ui->checkBox_removeDarkH->setChecked(grabber->getRemovingDarkH());
}
