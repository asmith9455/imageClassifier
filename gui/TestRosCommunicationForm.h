#ifndef TESTROSCOMMUNICATIONFORM_H
#define TESTROSCOMMUNICATIONFORM_H

#include <QDialog>
//#include "ros/ros.h"

namespace Ui {
class TestRosCommunicationForm;
}

class TestRosCommunicationForm : public QDialog
{
    Q_OBJECT

public:
    explicit TestRosCommunicationForm(QWidget *parent = 0);
    ~TestRosCommunicationForm();

private slots:
    void on_pushButton_clicked();

private:
    Ui::TestRosCommunicationForm *ui;
};

#endif // TESTROSCOMMUNICATIONFORM_H
