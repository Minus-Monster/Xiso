#include "CGrabberDialog.h"
#include "ui_CGrabberDialog.h"
#include "CGrabber.h"
#include <QFileDialog>

CGrabberDialog::CGrabberDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CGrabberDialog)
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
    });

}

CGrabberDialog::~CGrabberDialog()
{
    delete ui;
}

void CGrabberDialog::setGrabber(CGrabber *_grabber)
{
    grabber = _grabber;
}
