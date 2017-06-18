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

    if (dbOpened)
    {
        database.close();
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
    }

    database.setDatabaseName(fileName);

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

    if (!database.open())
    {
        QMessageBox::critical(this, "Error", "Could not open a connection to the database.",
            QMessageBox::Ok);
        dbOpened = false;
        return;
    }
    dbOpened = true;

    ui->label_status->setText("Created and opened database " + database.databaseName());


}

void manageTrainingDataForm::on_btn_selectDatabase_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image Database"),
                               "C:/",
                               tr("(*.sqlite)"));


    if (dbOpened)
    {
        database.close();
    }

    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(fileName);

    if (!database.open())
    {
        QMessageBox::critical(this, "Error", "Could not open a connection to the database.",
            QMessageBox::Ok);
        dbOpened = false;
        return;
    }
    dbOpened = true;

    updateAllTables();

    ui->label_status->setText("Selected database " + database.databaseName());

}






void manageTrainingDataForm::storeCaptureDevice(QString name, int imageWidth, int imageHeight, int bytesPerPixel)
{

    QSqlQuery qry;

    qry.prepare("INSERT INTO captureDevices (name, imageWidth, imageHeight, bytesPerPixel)"
                    "VALUES(:W_name,:W_imageWidth,:W_imageHeight,:W_bytesPerPixel)");

    qry.bindValue(":W_name",name);
    qry.bindValue(":W_imageWidth",imageWidth);
    qry.bindValue(":W_imageHeight",imageHeight);
    qry.bindValue(":W_bytesPerPixel",bytesPerPixel);

    if (!qry.exec())
    {
        QMessageBox::critical(this, "Error", "Could not insert the capturedevice. Error is: " + qry.lastError().text(),
            QMessageBox::Ok);
        return;
    }
    ui->label_status->setText("Inserted capture device " + name + " into " + database.databaseName());

}

void manageTrainingDataForm::storeProperty(QString propertyName)
{
    QSqlQuery qry;

    qry.prepare("INSERT INTO properties (property)"
                    "VALUES(:W_name)");

    qry.bindValue(":W_name",propertyName);

    if (!qry.exec())
    {
        QMessageBox::critical(this, "Error", "Could not insert the property. Error is: " + qry.lastError().text(),
            QMessageBox::Ok);
        return;
    }
    ui->label_status->setText("Inserted property " + propertyName + " into " + database.databaseName());
}

QByteArray manageTrainingDataForm::mat2ByteArray(const cv::Mat &image)
{
    QByteArray byteArray;
    QDataStream stream( &byteArray, QIODevice::WriteOnly );
    stream << image.type();
    stream << image.rows;
    stream << image.cols;
    const size_t data_size = image.cols * image.rows * image.elemSize();
    QByteArray data = QByteArray::fromRawData( (const char*)image.ptr(), data_size );
    stream << data;
    return byteArray;
}

cv::Mat manageTrainingDataForm::byteArray2Mat(const QByteArray & byteArray)
{
    QDataStream stream(byteArray);
    int matType, rows, cols;
    QByteArray data;
    stream >> matType;
    stream >> rows;
    stream >> cols;
    stream >> data;
    cv::Mat mat( rows, cols, matType, (void*)data.data() );
    return mat.clone();
}

void manageTrainingDataForm::storeImage(int captureDeviceReference, cv::Mat image)
{
    QSqlQuery qry;

    qry.prepare("INSERT INTO images (image, captureDevice)"
                    "VALUES(:W_image, :W_captureDevice)");

    qry.bindValue(":W_image",manageTrainingDataForm::mat2ByteArray(image));
    qry.bindValue(":W_captureDevice",captureDeviceReference);

    if (!qry.exec())
    {
        QMessageBox::critical(this, "Error", "Could not insert the property. Error is: " + qry.lastError().text(),
            QMessageBox::Ok);
        return;
    }
    ui->label_status->setText("Inserted image into " + database.databaseName());
}





void manageTrainingDataForm::on_btn_addCaptureDevice_clicked()
{
    QString name = ui->lineEdit_name->text();
    QString imageWidth = ui->lineEdit_imageWidth->text();
    QString imageHeight = ui->lineEdit_imageHeight->text();
    QString bytesPerPixel = ui->lineEdit_bytesPerPixel->text();

    storeCaptureDevice(name, imageWidth.toInt(), imageHeight.toInt(), bytesPerPixel.toInt());

    updateCaptureDeviceTableFromDb();

}

void manageTrainingDataForm::on_btn_addProperty_clicked()
{
    QString name = ui->lineEdit_propertyName->text();

    storeProperty(name);

    updatePropertiesTableFromDb();
}

void manageTrainingDataForm::on_btn_selectImageToAdd_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image Database"),
                               "C:/",
                               tr("Images(*.png *.bmp *.tiff *.jpg *.gif);;Any(*.*)"));


    imgToStore = cv::imread(fileName.toStdString(), CV_LOAD_IMAGE_COLOR);
    ui->label_selectedPic->setPixmap(QPixmap(fileName));
    ui->label_selectedPic->setScaledContents(true);

    ui->label_status->setText("Loaded picture at " + fileName );
    ui->label_imageStatus->setText("Picture from disk: " + fileName);

}

void manageTrainingDataForm::on_btn_addImgToDb_clicked()
{
    QItemSelectionModel *select = ui->tableView_captureDevicesForImage->selectionModel();

    bool selectionMade = select->hasSelection();

    if (!selectionMade)
    {
        QMessageBox::information(this, "Information", "Please select a capture device.",
            QMessageBox::Ok);
        return;
    }

    QModelIndexList rows = select->selectedRows();

    QModelIndex index = rows.at(0); //get the first selected capture device

    int id = index.sibling(index.row(), 0).data().toInt();    //get the id (primary key) of the capture device to delete - this will be used as the foreign key in the images table

    storeImage(id, this->imgToStore);


    ui->label_status->setText("Added image to database at " + database.databaseName());
}







void manageTrainingDataForm::updateAllTables()
{
    updateCaptureDeviceTableFromDb();
    updatePropertiesTableFromDb();
    updateImagesTableFromDb();
}

void manageTrainingDataForm::updateCaptureDeviceTableFromDb()
{

    QSqlTableModel *model = new QSqlTableModel(this,database);
    model->setTable("captureDevices");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(0, Qt::Horizontal, tr("id"));
    model->setHeaderData(1, Qt::Horizontal, tr("name"));
    model->setHeaderData(2, Qt::Horizontal, tr("imageWidth"));
    model->setHeaderData(3, Qt::Horizontal, tr("imageHeight"));
    model->setHeaderData(4, Qt::Horizontal, tr("bytesPerPixel"));
    ui->tableView_captureDevices->setModel(model);
    ui->tableView_captureDevicesForImage->setModel(model);

}

void manageTrainingDataForm::updatePropertiesTableFromDb()
{

    QSqlTableModel *model = new QSqlTableModel(this,database);
    model->setTable("properties");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(0, Qt::Horizontal, tr("id"));
    model->setHeaderData(1, Qt::Horizontal, tr("property"));
    ui->tableView_properties->setModel(model);

}

void manageTrainingDataForm::updateImagesTableFromDb()
{
    QSqlTableModel *model = new QSqlTableModel(this,database);
    model->setTable("images");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(0, Qt::Horizontal, tr("id"));
    model->setHeaderData(1, Qt::Horizontal, tr("image"));
    model->setHeaderData(2, Qt::Horizontal, tr("captureDevice"));
    ui->tableView_images->setModel(model);
}






void manageTrainingDataForm::on_btn_deleteSelectedCaptureDevice_clicked()
{
    QItemSelectionModel *select = ui->tableView_captureDevices->selectionModel();

    bool selectionMade = select->hasSelection();

    if (!selectionMade)
    {
        QMessageBox::information(this, "Information", "No selection made - nothing will be deleted.",
            QMessageBox::Ok);
        return;
    }

    std::vector<QString> deletedDevs;

    QModelIndexList rows = select->selectedRows();

    for(int i=0; i< rows.count(); i++)
    {
        QModelIndex index = rows.at(i); //get the current row
        int id = index.sibling(index.row(), 0).data().toInt();    //get the id (primary key) of the capture device to delete
        QString name = index.sibling(index.row(), 1).data().toString();
        deletedDevs.push_back(name);

        //run an sql query to delete the device
        QString sqlqstr = "DELETE FROM captureDevices WHERE id = " + QString::number(id) + ";";

        QSqlQuery tmp = database.exec(sqlqstr);

        if (!tmp.exec())
            QMessageBox::critical(this, "Error", "Could not delete the capturedevice. Error is: " + tmp.lastError().text(),
                QMessageBox::Ok);

    }

    updateCaptureDeviceTableFromDb();

    QString statusMessage = "Deleted ";

    for(size_t i = 0; i < deletedDevs.size(); i++)
    {
        if (i == deletedDevs.size()-1 && i>0)
            statusMessage += " and ";

        statusMessage += deletedDevs[i];

        if (i != deletedDevs.size()-1 && deletedDevs.size()>2)
            statusMessage += ", ";
    }
    statusMessage += " from " + database.databaseName();\

    ui->label_status->setText(statusMessage);

}

void manageTrainingDataForm::on_btn_deleteProperty_clicked()
{
    QItemSelectionModel *select = ui->tableView_properties->selectionModel();

    bool selectionMade = select->hasSelection();

    if (!selectionMade)
    {
        QMessageBox::information(this, "Information", "No selection made - nothing will be deleted.",
            QMessageBox::Ok);
        return;
    }

    std::vector<QString> deletedProps;

    QModelIndexList rows = select->selectedRows();

    for(int i=0; i< rows.count(); i++)
    {
        QModelIndex index = rows.at(i); //get the current row
        int id = index.sibling(index.row(), 0).data().toInt();    //get the id (primary key) of the property to delete
        QString name = index.sibling(index.row(), 1).data().toString();
        deletedProps.push_back(name);

        //run an sql query to delete the device
        QString sqlqstr = "DELETE FROM properties WHERE id = " + QString::number(id) + ";";

        QSqlQuery tmp = database.exec(sqlqstr);

        if (!tmp.exec())
            QMessageBox::critical(this, "Error", "Could not delete the property. Error is: " + tmp.lastError().text(),
                QMessageBox::Ok);

    }

    updatePropertiesTableFromDb();

    QString statusMessage = "Deleted ";

    for(size_t i = 0; i < deletedProps.size(); i++)
    {
        if (i == deletedProps.size()-1 && i>0)
            statusMessage += " and ";

        statusMessage += deletedProps[i];

        if (i != deletedProps.size()-1 && deletedProps.size()>2)
            statusMessage += ", ";
    }
    statusMessage += " from " + database.databaseName();\

    ui->label_status->setText(statusMessage);
}





void manageTrainingDataForm::on_tableView_images_doubleClicked(const QModelIndex &index)
{

    int id = index.sibling(index.row(), 0).data().toInt();
    QByteArray bytes = index.sibling(index.row(), 1).data().toByteArray();
    int capDevice = index.sibling(index.row(), 2).data().toInt();



    cv::Mat img = manageTrainingDataForm::byteArray2Mat(bytes);



    ui->label_status->setText("Opened image "+QString(id)+" from the images table in " + database.databaseName());



    std::string title = "Image ID: "+ std::to_string(id) + ", Capture Device ID: " + std::to_string(capDevice);
    cv::resize(img,img,cv::Size(1280,720));
    cv::imshow(title,img);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
