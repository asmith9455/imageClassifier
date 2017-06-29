#ifndef TESTROSCOMMUNICATIONFORM_H
#define TESTROSCOMMUNICATIONFORM_H

#include <QDialog>

namespace Ui {
class TestRosCommunicationForm;
}

class TestRosCommunicationForm : public QDialog
{
    Q_OBJECT

public:
    explicit TestRosCommunicationForm(QWidget *parent = 0);
    ~TestRosCommunicationForm();

private:
    Ui::TestRosCommunicationForm *ui;
};

#endif // TESTROSCOMMUNICATIONFORM_H
