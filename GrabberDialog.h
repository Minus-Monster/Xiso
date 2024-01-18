#ifndef GRABBERDIALOG_H
#define GRABBERDIALOG_H

#include <QDialog>
#include <QTreeWidget>

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
    void widgetGenerator(QString mcfPath);
    void generateChildrenWidgetItem(QTreeWidgetItem *parent, QString children);

public slots:
    void updateInformation();

private:
    Ui::GrabberDialog *ui;
    Grabber *grabber =nullptr;
    QString currentTestFilePath = "";

    QTreeWidget *widget;
    QList<QTreeWidgetItem*> manageItems;
};

#endif // GRABBERDIALOG_H
