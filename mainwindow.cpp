#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "manageTrainingDataForm.h"
#include <ClassifyCameraStreamForm.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    manageTrainingDataForm w;
    w.exec();
}

void MainWindow::on_btn_classifyCameraStream_clicked()
{
#ifndef TX1
    QMessageBox::question(this, "Unsuitable Hardware", "This function only works when the software has been compiled on the Jetson TX1 (or another device that has an appropriate device that outputs its feed in a way such that gstreamer can access it.",
        QMessageBox::Ok);
#endif // TX1
    ClassifyCameraStreamForm ccsf;
    ccsf.exec();
}
