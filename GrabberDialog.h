#ifndef GRABBERDIALOG_H
#define GRABBERDIALOG_H

#include <QDialog>

namespace Ui {
class GrabberDialog;
}

class Grabber;
class GrabberDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GrabberDialog(QWidget *parent = nullptr);
    ~GrabberDialog();
    void setGrabber(Grabber *_grabber);

public slots:
    void updateInformation();

private:
    Ui::GrabberDialog *ui;
    Grabber *grabber =nullptr;
    QString currentTestFilePath = "";
};

#endif // GRABBERDIALOG_H
