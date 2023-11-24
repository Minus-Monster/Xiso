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
    ui->horizontalScrollBar_bright->setRange(0, 10);
    contrast = 6;
    ui->horizontalScrollBar_contrast->setRange(0, 20);

    connect(ui->horizontalScrollBar_min, &QScrollBar::valueChanged, this, [this](int val){
    });
    connect(ui->horizontalScrollBar_max, &QScrollBar::valueChanged, this, [this](int val){
    });
    connect(ui->horizontalScrollBar_bright, &QScrollBar::valueChanged, this, [this](int val){
        this->bright = ui->horizontalScrollBar_bright->value();
        convertImage(current);
    });
    connect(ui->horizontalScrollBar_contrast, &QScrollBar::valueChanged, this, [this](int val){
        this->bright = ui->horizontalScrollBar_contrast->value();
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
}
