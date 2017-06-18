#include "manageTrainingDataForm.h"
#include "ui_manageTrainingDataForm.h"

manageTrainingDataForm::manageTrainingDataForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::manageTrainingDataForm)
{
    ui->setupUi(this);
}

manageTrainingDataForm::~manageTrainingDataForm()
{
    delete ui;
}

int manageTrainingDataForm::nullCallback(void *NotUsed, int argc, char **argv, char **azColName) {

    return 0;
}

void manageTrainingDataForm::on_btn_createImgDb_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Create Image Database"),
                               "C:/",
                               tr("(*.sqlite)"));

    sqlite3 *db;
    int rc; // This line
    const char* sql; // This line

    rc = sqlite3_open(fileName.toLatin1().data(), &db);

    sqlite3_stmt *dat = NULL;
    char** errMsg = NULL;
    sql = "PRAGMA foreign_keys = ON;";
    rc = sqlite3_exec(db, sql, &manageTrainingDataForm::nullCallback, dat, errMsg);

    if (rc != SQLITE_OK)
    {
        QString errMsg = QString("Failed to enable foreign keys in database.");
        QMessageBox::question(this, "Error", errMsg,
            QMessageBox::Ok);
    }







    sql ="CREATE TABLE captureDevices("  \
         "id                                        INTEGER PRIMARY KEY,    "\
         "name                                      TEXT NOT NULL,                   "\
         "imageWidth                                INTEGER NOT NULL,                "\
         "imageHeight                               INTEGER NOT NULL,                "\
         "bytesPerPixel                             INTEGER NOT NULL, "\
         "CONSTRAINT uniqueDevices UNIQUE (name, imageWidth, imageHeight, bytesPerPixel));                ";

    rc = sqlite3_exec(db, sql, &manageTrainingDataForm::nullCallback, dat, errMsg);
    if (rc != SQLITE_OK)
    {
        QString errMsg = QString("Failed to create table (captureDevices) in database.");
        QMessageBox::question(this, "Error", errMsg,
            QMessageBox::Ok);
    }







    sql = "CREATE TABLE properties("\
                "id                                       INTEGER PRIMARY KEY NOT NULL,"\
                "property                                 TEXT NOT NULL"
                ");";

    rc = sqlite3_exec(db, sql, &manageTrainingDataForm::nullCallback, dat, errMsg);


    if (rc != SQLITE_OK)
        QMessageBox::question(this, "Error", "Failed to create table (propertyList) in database.",
            QMessageBox::Ok);










    sql = "CREATE TABLE images("\
                "id                                       INTEGER PRIMARY KEY NOT NULL,"\
                "image                                    BLOB NOT NULL,"\
                "captureDate                              TEXT NOT NULL,"\
                "captureDevice                            INTEGER,"\
                "FOREIGN KEY(captureDevice) REFERENCES 	  captureDevices(id)"\
                ");";

    rc = sqlite3_exec(db, sql, &manageTrainingDataForm::nullCallback, dat, errMsg);


    if (rc != SQLITE_OK)
        QMessageBox::question(this, "Error", "Failed to create table (images) in database.",
            QMessageBox::Ok);






    //we will just store the lists of contour points in a string - this seems easier at the moment
    sql ="CREATE TABLE segmentedRegions("  \
         "id                                        INTEGER PRIMARY KEY,          "\
         "imageReference                            INTEGER,                      "\
         "outerContour                              TEXT NOT NULL,                "\
         "innerContours                             TEXT,                         "
         "FOREIGN KEY(imageReference) REFERENCES    images(id)                    "\
         ");";

    /* Create SQL statement */

    rc = sqlite3_exec(db, sql, &manageTrainingDataForm::nullCallback, dat, errMsg);

    if (rc != SQLITE_OK)
    {
        QString errMsg = QString("Failed to create table (segmentedRegions) in database.");
        QMessageBox::question(this, "Error", errMsg,
            QMessageBox::Ok);
    }











    sql ="CREATE TABLE regionProperties("  \
            "id                                                 INTEGER PRIMARY KEY,    "\
            "propertyReference                                  INTEGER NOT NULL,       "\
            "segmentedRegionReference                           INETEGER NOT NULL,"\
            "FOREIGN KEY(propertyReference) REFERENCES          properties(id),"\
            "FOREIGN KEY(segmentedRegionReference) REFERENCES   segmentedRegions(id)"\
            ");";


    rc = sqlite3_exec(db, sql, &manageTrainingDataForm::nullCallback, dat, errMsg);

    if (rc != SQLITE_OK)
    {
        QString errMsg = QString("Failed to create table (regionProperties) in database.");
        QMessageBox::question(this, "Error", errMsg,
            QMessageBox::Ok);
    }



    sqlite3_close(db);

    this->currentDbPath = fileName;

}

void manageTrainingDataForm::storeCaptureDevice(QString name, QString imageWidth, QString imageHeight, QString bytesPerPixel)
{
    sqlite3 *db;
    int rc;

    rc = sqlite3_open(this->currentDbPath.toLatin1().data(), &db);

    sqlite3_stmt *dat = NULL;
    char** errMsg = NULL;
    QString sqlqstr = "INSERT INTO captureDevices (name, imageWidth, imageHeight, bytesPerPixel) VALUES (\""
            + name + "\", "
            + imageWidth + ", "
            + imageHeight + ", "
            + bytesPerPixel+");";



    rc = sqlite3_exec(db, sqlqstr.toLatin1().data(), &manageTrainingDataForm::nullCallback, dat, errMsg);

    if (rc != SQLITE_OK)
    {
        QString errMsg2;
        if (rc == 19)
            errMsg2 = QString("Identical capture device already exists in database.");
        else
            errMsg2 = QString("Failed to add the capture device to the database. Make sure the database exists.");
        QMessageBox::critical(this, "Error", errMsg2,
            QMessageBox::Ok);
    }

    sqlite3_close(db);
}

void manageTrainingDataForm::on_btn_addCaptureDevice_clicked()
{
    QString name = ui->lineEdit_name->text();
    QString imageWidth = ui->lineEdit_imageWidth->text();
    QString imageHeight = ui->lineEdit_imageHeight->text();
    QString bytesPerPixel = ui->lineEdit_bytesPerPixel->text();

    storeCaptureDevice(name, imageWidth, imageHeight, bytesPerPixel);

    updateCaptureDeviceTableFromDb();

}

void manageTrainingDataForm::on_btn_selectDatabase_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image Database"),
                               "C:/",
                               tr("(*.sqlite)"));

    this->currentDbPath = fileName;

    QSqlDatabase test = QSqlDatabase::addDatabase("QSQLITE");
    test.setDatabaseName(fileName);

    updateCaptureDeviceTableFromDb();

}

void manageTrainingDataForm::updateCaptureDeviceTableFromDb()
{
    QSqlDatabase test = QSqlDatabase::addDatabase("QSQLITE");
    test.setDatabaseName(this->currentDbPath);

    if (!test.open())
    {
        QMessageBox::critical(this, "Error", "Could not update capture device table.",
            QMessageBox::Ok);
        return;
    }

    QSqlTableModel *model = new QSqlTableModel(this,test);
    model->setTable("captureDevices");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(0, Qt::Horizontal, tr("id"));
    model->setHeaderData(1, Qt::Horizontal, tr("name"));
    model->setHeaderData(2, Qt::Horizontal, tr("imageWidth"));
    model->setHeaderData(3, Qt::Horizontal, tr("imageHeight"));
    model->setHeaderData(4, Qt::Horizontal, tr("bytesPerPixel"));
    ui->tableView_captureDevices->setModel(model);

    test.close();

}

void manageTrainingDataForm::on_btn_deleteSelectedCaptureDevice_clicked()
{
    QItemSelectionModel *select = ui->tableView_captureDevices->selectionModel();

    bool selectionMade = select->hasSelection();

    if (!selectionMade)
    {
        QMessageBox::information(this, "Information", "No selection made - nothing will be deleted.",
            QMessageBox::Ok);
    }

    QModelIndexList rows = select->selectedRows();

    for(int i=0; i< rows.count(); i++)
    {
        QModelIndex index = rows.at(i); //get the current row
        int id = index.sibling(index.row(), 0).data().toInt();    //get the id (primary key) of the capture device to delete

        //run an sql query to delete the device
        sqlite3 *db;
        int rc;

        rc = sqlite3_open(this->currentDbPath.toLatin1().data(), &db);

        sqlite3_stmt *dat = NULL;
        char** errMsg = NULL;

        QString sqlqstr = "DELETE FROM captureDevices WHERE id = " + QString::number(id) + ";";



        rc = sqlite3_exec(db, sqlqstr.toLatin1().data(), &manageTrainingDataForm::nullCallback, dat, errMsg);

        if (rc != SQLITE_OK)
        {
            QString errMsg2;
            errMsg2 = QString("Deletion failed.");
            QMessageBox::critical(this, "Error", errMsg2,
                QMessageBox::Ok);
        }

        sqlite3_close(db);

    }

    updateCaptureDeviceTableFromDb();



    //select->selectedColumns();
}
