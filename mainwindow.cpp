#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Acquisition/GrabberWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    layout = new QVBoxLayout;
    ui->frame->setLayout(layout);

    widget = new Qylon::GraphicsWidget;
    widget->initialize();
    layout->addWidget(widget);


    connect(ui->actionGrabber, &QAction::triggered, this, [this](){
        this->grabber->getWidget()->show();
    });
    connect(ui->actionPanel, &QAction::triggered, this, [this](bool on){
        this->ui->dockWidget_panel->setVisible(on);
    });
    connect(ui->dockWidget_panel, &QDockWidget::visibilityChanged, this, [this](bool on){
        this->ui->actionPanel->setChecked(on);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setGrabber(Qylon::Grabber *g)
{
    connect(g, &Qylon::Grabber::sendImage, this, [this](QImage image){
        this->widget->setImage(image);
    });
//    emit g->sendImage(QImage("C:/Users/MPark/Documents/Raw.tif"));
    grabber = g;
    Qylon::GrabberWidget *widget = reinterpret_cast<Qylon::GrabberWidget*>(grabber->getWidget());
    widget->setDefaultAppletPath("C:/Users/User/Desktop/10G/SimpleTest/Inno_5608_ImpulseCX4s_CZ_131023_sangrae.hap");
    widget->setDefaultConfigPath("C:/Users/User/Desktop/10G/SimpleTest/Inno.mcf");
}

void MainWindow::setDetector(Detector *d)
{

}

