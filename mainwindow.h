#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <ClassifyCameraStreamForm.h>
#include <TrainTextureClassifierForm.h>
#include <TestRosCommunicationForm.h>
#include <manageTrainingDataForm.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btn_manageTrainingImages_clicked();

    void on_btn_classifyCameraStream_clicked();

    void on_btn_trainTextureClassifier_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    std::vector<std::shared_ptr<manageTrainingDataForm>> manageTrainingDataForms;
    std::vector<std::shared_ptr<ClassifyCameraStreamForm>> classifyCameraStreamForms;
    std::vector<std::shared_ptr<TrainClassifierForm>> trainClassifierForms;
    std::vector<std::shared_ptr<TestRosCommunicationForm>> rosCommForms;
};

#endif // MAINWINDOW_H
