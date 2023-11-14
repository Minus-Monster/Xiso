#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    layout = new QVBoxLayout;
    widget = new Qylon::GraphicsWidget;
    widget->initialize();
    layout->addWidget(widget);
    ui->frame->setLayout(layout);

//    widget->setFit(true);

    connect(ui->actionPanel, &QAction::triggered, this, [this](bool on){this->ui->dockWidget_panel->setVisible(on);});
    connect(ui->dockWidget_panel, &QDockWidget::visibilityChanged, this, [this](bool on){this->ui->actionPanel->setChecked(on);});
    connect(ui->pushButton_Sequential, &QPushButton::clicked, this, [this](){
        int cnt = ui->spinBox_Sequential->value();
        grabber->sequentialGrabbing(cnt);
        camera->sequentialGrab(cnt+1);

//        detector->setExposureTime(1000);
//        detector->sequentialGrabbing(cnt+1);
    });
    connect(ui->pushButton_Continuous, &QPushButton::clicked, this, [this](bool on){
        if(on){
            grabber->continuousGrabbing();
            camera->continuousGrab();
        }else{
            grabber->stopGrabbing();
            camera->stopGrab();
        }
    });
    connect(ui->pushButton_ResetGrabber, &QPushButton::clicked, this, [this](){
        this->grabber->stopGrabbing();
    });
    connect(ui->pushButton_dark, &QPushButton::clicked, this, [this](){
        grabber->setCalibMode(false);
        // Grabbing x images

        // need to capture X frames and save that frames in the specific folder.

    });
    connect(ui->pushButton_bright, &QPushButton::clicked, this, [this](){
        grabber->setCalibMode(false);
        // Grabbing x images
    });
    connect(ui->pushButton_out, &QPushButton::clicked, this, [this](){
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
}

MainWindow::~MainWindow()
{
    delete ui;
}


/// GRABBER PART BEGINS ///
void MainWindow::setGrabber(CGrabber *c)
{
    grabber = c;
    grabber->loadApplet("C:/Program Files/Basler/FramegrabberSDK/Hardware Applets/IMP-CX-4S/Innometry_10G_Project.hap");
    grabber->loadConfiguration("C:/Users/User/Desktop/10G/SimpleTest/Inno.mcf");
    grabber->initialize();
    connect(grabber, &CGrabber::sendImage, this, [this](QImage image){
        this->widget->setImage(image);
    });
}
/// DETECTOR PART BEGINS ///
void MainWindow::setDetector(Detector *d)
{
    detector = d;
    detector->initialize();
    detector->setROI(2400, 600, 0, 0);

    connect(ui->spinBox_Exposure, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){

    });
}

void MainWindow::setCamera(Qylon::Camera *c)
{
    camera = c;
    camera->openCamera("Basler acA2440-20gc (23131022)");
    connect(camera, &Qylon::Camera::grabbed, this, [this](){
        qDebug() << "Grabbed from camera";
        camera->drawLock();
        const void* img = camera->getBuffer();
        grabber->convertToGrabberImage((unsigned short*)img);
    });
}



