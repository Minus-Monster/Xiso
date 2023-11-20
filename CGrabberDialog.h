#ifndef CGRABBERDIALOG_H
#define CGRABBERDIALOG_H

#include <QDialog>

namespace Ui {
class CGrabberDialog;
}

class CGrabber;
class CGrabberDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CGrabberDialog(QWidget *parent = nullptr);
    ~CGrabberDialog();
    void setGrabber(CGrabber *_grabber);
    void updateInformation();

private:
    Ui::CGrabberDialog *ui;
    CGrabber *grabber =nullptr;
};

#endif // CGRABBERDIALOG_H
