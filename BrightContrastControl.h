#ifndef BRIGHTCONTRASTCONTROL_H
#define BRIGHTCONTRASTCONTROL_H

#include <QDialog>

namespace Ui {
class BrightContrastControl;
}

class BrightContrastControl : public QDialog
{
    Q_OBJECT

public:
    explicit BrightContrastControl(QWidget *parent = nullptr);
    ~BrightContrastControl();
    QImage convertImage(QImage *img);

signals:
    void converted(QImage image);

private:
    Ui::BrightContrastControl *ui;
    int min=0;
    int max=0;
    int bright=0;
    int contrast=0;
    QImage *current;
};

#endif // BRIGHTCONTRASTCONTROL_H
