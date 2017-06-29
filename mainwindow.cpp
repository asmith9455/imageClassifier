#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "manageTrainingDataForm.h"


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

void MainWindow::on_btn_manageTrainingImages_clicked()
{
    manageTrainingDataForm w;
    w.exec();
}

void MainWindow::on_btn_classifyCameraStream_clicked()
{
    ClassifyCameraStreamForm ccsf;
    ccsf.exec();
}

void MainWindow::on_btn_trainTextureClassifier_clicked()
{
    TrainClassifierForm tcf;
    tcf.exec();
}

void MainWindow::on_pushButton_clicked()
{
    TestRosCommunicationForm trcf;
    trcf.exec();
}
