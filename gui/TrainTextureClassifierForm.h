#ifndef TRAINCLASSIFIERFORM_H
#define TRAINCLASSIFIERFORM_H

#include <QDialog>
#include <QSqlDatabase>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <TrainingImageDbWrapper.h>
#include <ImageSequence.h>

namespace Ui {
class TrainClassifierForm;
}

class TrainClassifierForm : public QDialog
{
    Q_OBJECT

public:
    explicit TrainClassifierForm(QWidget *parent = 0);
    ~TrainClassifierForm();

private slots:
    void on_btn_generateImages_clicked();

private:
    Ui::TrainClassifierForm *ui;
    void updatePropertiesFromDb();
    QString dbPath;
};

#endif // TRAINCLASSIFIERFORM_H
