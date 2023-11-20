#ifndef DETECTORDIALOG_H
#define DETECTORDIALOG_H

#include <QDialog>

namespace Ui {
class DetectorDialog;
}
class Detector;
class DetectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DetectorDialog(QWidget *parent = nullptr);
    ~DetectorDialog();

    void setDetector(Detector *d){
        detector = d;
    }

private:
    Ui::DetectorDialog *ui;
    Detector *detector;
};

#endif // DETECTORDIALOG_H
