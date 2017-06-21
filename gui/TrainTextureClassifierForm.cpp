#include "TrainTextureClassifierForm.h"
#include "ui_TrainClassifierForm.h"

TrainClassifierForm::TrainClassifierForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrainClassifierForm)
{
    ui->setupUi(this);
    updatePropertiesFromDb();
}

TrainClassifierForm::~TrainClassifierForm()
{
    delete ui;
}

void TrainClassifierForm::updatePropertiesFromDb()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image Database"),
                               "C:/",
                               tr("(*.sqlite)"));

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(fileName);

    if (!database.open())
    {
        QMessageBox::critical(this, "Error", "Could not open a connection to the database.",
            QMessageBox::Ok);
        return;
    }

    database.exec("PRAGMA foreign_keys = ON;");

    QSqlTableModel *model = new QSqlTableModel(this,database);
    model->setTable("properties");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(0, Qt::Horizontal, tr("id"));
    model->setHeaderData(1, Qt::Horizontal, tr("property"));
    ui->tableView_properties->setModel(model);

    database.close();
}
