#ifndef TRAINCLASSIFIERFORM_H
#define TRAINCLASSIFIERFORM_H

#include <QDialog>
#include <QSqlDatabase>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlTableModel>

namespace Ui {
class TrainClassifierForm;
}

class TrainClassifierForm : public QDialog
{
    Q_OBJECT

public:
    explicit TrainClassifierForm(QWidget *parent = 0);
    ~TrainClassifierForm();

private:
    Ui::TrainClassifierForm *ui;
    void updatePropertiesFromDb();
};

#endif // TRAINCLASSIFIERFORM_H
