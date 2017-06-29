#include "TestRosCommunicationForm.h"
#include "ui_TestRosCommunicationForm.h"

TestRosCommunicationForm::TestRosCommunicationForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestRosCommunicationForm)
{
    ui->setupUi(this);
}

TestRosCommunicationForm::~TestRosCommunicationForm()
{
    delete ui;
}

void TestRosCommunicationForm::on_pushButton_clicked()
{
    ui->label_commStatus->setText("Status: Communicating");
}
