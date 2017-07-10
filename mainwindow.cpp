#include "mainwindow.h"
#include "ui_mainwindow.h"



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
    std::shared_ptr<manageTrainingDataForm> tmp = std::make_shared<manageTrainingDataForm>();
    manageTrainingDataForms.push_back(tmp);
    tmp->show();
}

void MainWindow::on_btn_classifyCameraStream_clicked()
{
    std::shared_ptr<ClassifyCameraStreamForm> tmp = std::make_shared<ClassifyCameraStreamForm>();
    classifyCameraStreamForms.push_back(tmp);
    tmp->show();
}

void MainWindow::on_btn_trainTextureClassifier_clicked()
{
    std::shared_ptr<TrainClassifierForm> tmp = std::make_shared<TrainClassifierForm>();
    trainClassifierForms.push_back(tmp);
    tmp->show();
}

void MainWindow::on_pushButton_clicked()
{
    std::shared_ptr<TestRosCommunicationForm> tmp = std::make_shared<TestRosCommunicationForm>();
    rosCommForms.push_back(tmp);
    tmp->show();
}
