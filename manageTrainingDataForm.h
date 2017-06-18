#ifndef MANAGETRAININGDATAFORM_H
#define MANAGETRAININGDATAFORM_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <sqlite3.h>
#include <iostream>
#include <QtCore>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>

namespace Ui {
class manageTrainingDataForm;
}

class manageTrainingDataForm : public QDialog
{
    Q_OBJECT

public:
    explicit manageTrainingDataForm(QWidget *parent = 0);
    ~manageTrainingDataForm();

private slots:
    void on_btn_createImgDb_clicked();

    void on_btn_addCaptureDevice_clicked();

    void on_btn_selectDatabase_clicked();

    void on_btn_deleteSelectedCaptureDevice_clicked();

private:
    Ui::manageTrainingDataForm *ui;
    static int nullCallback(void *NotUsed, int argc, char **argv, char **azColName);
    void storeCaptureDevice(QString name, QString imageWidth, QString imageHeight, QString bitsPerPixel);
    QString currentDbPath;
    void updateCaptureDeviceTableFromDb();
};

#endif // MANAGETRAININGDATAFORM_H
