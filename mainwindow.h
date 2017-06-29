#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <ClassifyCameraStreamForm.h>
#include <TrainTextureClassifierForm.h>
#include <TestRosCommunicationForm.h>

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
};

#endif // MAINWINDOW_H
