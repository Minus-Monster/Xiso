#include "BrightContrastControl.h"
#include "ui_BrightContrastControl.h"
#include <QImage>
#include <QDebug>

BrightContrastControl::BrightContrastControl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrightContrastControl)
{
    ui->setupUi(this);
    min = 0;
    max = 65535;
    bright = 3;
    contrast = 6;
    ui->horizontalScrollBar_min->setRange(0, 65535);
    ui->horizontalScrollBar_max->setRange(0, 65535);
    ui->horizontalScrollBar_bright->setRange(0, 10);
    ui->horizontalScrollBar_contrast->setRange(0, 20);
    ui->horizontalScrollBar_min->setValue(min);
    ui->horizontalScrollBar_max->setValue(max);
    ui->horizontalScrollBar_bright->setValue(bright);
    ui->horizontalScrollBar_contrast->setValue(contrast);
    ui->spinBox_min->setValue(min);
    ui->spinBox_max->setValue(max);
    ui->spinBox_brightness->setValue(bright);
    ui->spinBox_contrast->setValue(contrast);

    connect(ui->horizontalScrollBar_min, &QScrollBar::valueChanged, this, [this](int val){
        this->min = ui->horizontalScrollBar_min->value();
    });
    connect(ui->horizontalScrollBar_max, &QScrollBar::valueChanged, this, [this](int val){
        this->max = ui->horizontalScrollBar_max->value();
    });
    connect(ui->horizontalScrollBar_bright, &QScrollBar::valueChanged, this, [this](int val){
        this->bright = ui->horizontalScrollBar_bright->value();
    });
    connect(ui->horizontalScrollBar_contrast, &QScrollBar::valueChanged, this, [this](int val){
        this->contrast = ui->horizontalScrollBar_contrast->value();
    });
    connect(ui->pushButton_apply, &QPushButton::clicked, this, [this](){
        convertImage(current);
    });
}

BrightContrastControl::~BrightContrastControl()
{
    delete ui;
}

QImage BrightContrastControl::convertImage(QImage *img)
{
    current = img;
    if(ui->groupBox_enable->isChecked()){
        qDebug() << "Try to convert the image" << min << max << bright << contrast;
        QImage newConverted(img->size(), QImage::Format_Grayscale16);
        for(int y=0; y < img->height(); ++y){
            for(int x=0; x< img->width(); ++x){
                QRgb pixel = img->pixel(x,y);

                int newR = std::clamp(qRed(pixel) + bright, min, max);
                int newG= std::clamp(qGreen(pixel) + bright, min, max);
                int newB= std::clamp(qBlue(pixel) + bright, min, max);

                newR = std::clamp(static_cast<int>(newR * contrast), min, max);
                newG = std::clamp(static_cast<int>(newG * contrast), min, max);
                newB = std::clamp(static_cast<int>(newB * contrast), min, max);
                newConverted.setPixel(x,y, qRgb(newR,newG,newB));
            }
        }
        emit converted(newConverted);
        return newConverted;
    }else{
        return *current;
    }
}
