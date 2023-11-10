#include "mainwindow.h"
#include "ui_mainwindow.h"

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
}

void MainWindow::setDetector(Detector *d)
{

}

