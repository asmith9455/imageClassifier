#include "manageTrainingDataForm.h"
#include "ui_manageTrainingDataForm.h"

manageTrainingDataForm::manageTrainingDataForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::manageTrainingDataForm)
{
    ui->setupUi(this);

    QObject::connect(ui->label_picToDrawOn, SIGNAL(clicked(int)),
                         this, SLOT(on_label_picToDrawOn_clicked(int)));

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
         "bitsPerPixel                              INTEGER NOT NULL, "\
         "CONSTRAINT uniqueDevices UNIQUE (name, imageWidth, imageHeight, bitsPerPixel));                ";

    rc = sqlite3_exec(db, sql, &manageTrainingDataForm::nullCallback, dat, errMsg);
    if (rc != SQLITE_OK)
    {
        QString errMsg = QString("Failed to create table (captureDevices) in database.");
        QMessageBox::question(this, "Error", errMsg,
            QMessageBox::Ok);
    }







    sql = "CREATE TABLE properties("\
                "id                                       INTEGER PRIMARY KEY NOT NULL,"\
                "property                                 TEXT NOT NULL,"\
                "CONSTRAINT uniqueProperties UNIQUE (property)"\
                ");";

    rc = sqlite3_exec(db, sql, &manageTrainingDataForm::nullCallback, dat, errMsg);


    if (rc != SQLITE_OK)
        QMessageBox::question(this, "Error", "Failed to create table (propertyList) in database.",
            QMessageBox::Ok);










    sql = "CREATE TABLE images("\
                "id                                       INTEGER PRIMARY KEY NOT NULL,"\
                "image                                    BLOB NOT NULL,"\
                "captureDevice                            INTEGER,"\
                "FOREIGN KEY(captureDevice) REFERENCES 	  captureDevices(id),"\
                "CONSTRAINT uniqueImages UNIQUE (image, captureDevice)"\
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
            "FOREIGN KEY(segmentedRegionReference) REFERENCES   segmentedRegions(id),"\
            "CONSTRAINT uniqueProperties UNIQUE (propertyReference, segmentedRegionReference)"\
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

    database.exec("PRAGMA foreign_keys = ON;");

    ui->label_status->setText("Created and opened database " + database.databaseName());

    updateAllTables();


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
    database.exec("PRAGMA foreign_keys = ON;");
    updateAllTables();

    ui->label_status->setText("Selected database " + database.databaseName());

}

void manageTrainingDataForm::on_label_picToDrawOn_clicked(int mouseX, int mouseY)
{
    //mouse co-ordinates relative to the top left corner of the image are mouseX and mouseY

    int imgX = (imgToDrawOn.cols * mouseX) / ui->label_picToDrawOn->width();
    int imgY = (imgToDrawOn.rows * mouseY) / ui->label_picToDrawOn->height();


    //we will store the locations for the image itself, rather than the locations on the image in the gui

    Point2<int> newPoint(imgX,imgY);

    if(currentDrawingMode == DRAWING_OUTER_CONTOUR)
        outerContour.push_back(newPoint);
    else if (currentDrawingMode == DRAWING_INNER_CONTOUR)
        innerContours.back().push_back(newPoint);



}



void manageTrainingDataForm::storeCaptureDevice(QString name, int imageWidth, int imageHeight, int bitsPerPixel)
{

    QSqlQuery qry;

    qry.prepare("INSERT INTO captureDevices (name, imageWidth, imageHeight, bitsPerPixel)"
                    "VALUES(:W_name,:W_imageWidth,:W_imageHeight,:W_bitsPerPixel)");

    qry.bindValue(":W_name",name);
    qry.bindValue(":W_imageWidth",imageWidth);
    qry.bindValue(":W_imageHeight",imageHeight);
    qry.bindValue(":W_bitsPerPixel",bitsPerPixel);

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
        ui->label_status->setText("Failed to insert property " + propertyName + " into " + database.databaseName());
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
    QString bitsPerPixel = ui->lineEdit_bitsPerPixel->text();

    storeCaptureDevice(name, imageWidth.toInt(), imageHeight.toInt(), bitsPerPixel.toInt());

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

    //always convert the image to 3 channel (BGR) - only tested with 1 byte per channel
    //alpha channel is discarded
    imgToStore = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);



    ui->label_selectedPic->setPixmap(QPixmap(fileName));


    ui->label_status->setText("Loaded picture at " + fileName );
    ui->label_imageStatus->setText("Picture from disk: " + fileName);

}

void manageTrainingDataForm::on_btn_addImgToDb_clicked()
{
    QItemSelectionModel *select = ui->tableView_captureDevicesForImage->selectionModel();

    if (select == NULL)
    {
        QMessageBox::information(this, "Information", "Please select a capture device.",
            QMessageBox::Ok);
        return;
    }

    bool selectionMade = select->hasSelection();

    if (!selectionMade)
    {
        QMessageBox::information(this, "Information", "Please select a capture device.",
            QMessageBox::Ok);
        return;
    }

    if (imgToStore.size().width == 0 && imgToStore.size().height == 0)
    {
        QMessageBox::information(this, "Information", "Please select an image.",
            QMessageBox::Ok);
        return;
    }

    QModelIndexList rows = select->selectedRows();

    QModelIndex index = rows.at(0); //get the first selected capture device

    int id = index.sibling(index.row(), 0).data().toInt();    //get the id (primary key) of the capture device to delete - this will be used as the foreign key in the images table

    int bitDepth;
    if(imgToStore.depth() == CV_8U)
        bitDepth = 8;
    else
    {
        QMessageBox::critical(this, "Error", "Don't know bit depth of the image you want to store. Source update required.",
            QMessageBox::Ok);
        return;
    }


    int numChannels = imgToStore.channels();


    int totalBits = bitDepth*numChannels;

    //totalBits should equal the number of bits per pixel specified in the capture device.

    QSqlQuery query;
    query.prepare("SELECT imageWidth, imageHeight, bitsPerPixel FROM captureDevices WHERE id = :W_id;");
    query.bindValue(":W_id", id);

    if(!query.exec())
    {
        QMessageBox::critical(this, "Error", "Could not get capture device information. Error is: " + query.lastError().text(),
            QMessageBox::Ok);
        return;
    }

    int imageWidth, imageHeight, bitsPerPixel;

    if( query.next() )
    {
        imageWidth = query.value(0).toInt();
        imageHeight = query.value(1).toInt();
        bitsPerPixel = query.value(2).toInt();
    }

    if (imageWidth != imgToStore.cols)
    {
        QMessageBox::critical(this, "Error", "Capture device image width doesn't match the image's width.",
            QMessageBox::Ok);
        return;
    }

    if (imageHeight != imgToStore.rows)
    {
        QMessageBox::critical(this, "Error", "Capture device image height doesn't match the image's height.",
            QMessageBox::Ok);
        return;
    }



    if (bitsPerPixel != totalBits)
    {
        QMessageBox::critical(this, "Error", "Capture device bit depth doesn't match the image's bit depth (with alpha removed).",
            QMessageBox::Ok);
        return;
    }

    storeImage(id, this->imgToStore);

    ui->label_status->setText("Added image to database at " + database.databaseName());

    updateImagesTableFromDb();
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
    model->setHeaderData(4, Qt::Horizontal, tr("bitsPerPixel"));
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
    ui->tableView_propertiesForSegRegions->setModel(model);

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
    ui->tableView_imagesForSegRegions->setModel(model);
}






void manageTrainingDataForm::on_btn_deleteSelectedCaptureDevice_clicked()
{
    QItemSelectionModel *select = ui->tableView_captureDevices->selectionModel();

    bool selectionMade = select->hasSelection();

    QModelIndexList rows = select->selectedRows();

    if (!selectionMade || rows.count() == 0)
    {
        QMessageBox::information(this, "Information", "No selection made - nothing will be deleted.",
            QMessageBox::Ok);
        return;
    }

    std::vector<QString> deletedDevs;



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
            QMessageBox::critical(this, "Error", "Could not delete capture device with id = " + QString(i) + ". Error is: " + tmp.lastError().text(),
                QMessageBox::Ok);

    }

    updateCaptureDeviceTableFromDb();

    QString statusMessage = "Deleted ";

    for(size_t i = 0; i < deletedDevs.size(); i++)
    {
        if (i == deletedDevs.size()-1 && i>0)
            statusMessage += "and ";

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

    QModelIndexList rows = select->selectedRows();

    if (!selectionMade || rows.count() == 0)
    {
        QMessageBox::information(this, "Information", "No selection made - nothing will be deleted.",
            QMessageBox::Ok);
        return;
    }

    std::vector<QString> deletedProps;





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
            QMessageBox::critical(this, "Error", "Could not delete property with id = " + QString(i) + ". Error is: " + tmp.lastError().text(),
                QMessageBox::Ok);

    }

    updatePropertiesTableFromDb();

    QString statusMessage = "Deleted ";

    for(size_t i = 0; i < deletedProps.size(); i++)
    {
        if (i == deletedProps.size()-1 && i>0)
            statusMessage += "and ";

        statusMessage += deletedProps[i];

        if (i != deletedProps.size()-1 && deletedProps.size()>2)
            statusMessage += ", ";
    }
    statusMessage += " from " + database.databaseName();\

    ui->label_status->setText(statusMessage);
}

void manageTrainingDataForm::on_btn_removeImgFromDb_clicked()
{

    QItemSelectionModel *select = ui->tableView_images->selectionModel();

    bool selectionMade = select->hasSelection();

    QModelIndexList rows = select->selectedRows();

    if (!selectionMade || rows.count() == 0)
    {
        QMessageBox::information(this, "Information", "No selection made - nothing will be deleted.",
            QMessageBox::Ok);
        return;
    }

    std::vector<QString> deletedImages;



    for(int i=0; i< rows.count(); i++)
    {
        QModelIndex index = rows.at(i); //get the current row
        int id = index.sibling(index.row(), 0).data().toInt();    //get the id (primary key) of the property to delete
        QString name = index.sibling(index.row(), 1).data().toString();
        deletedImages.push_back(name);

        //run an sql query to delete the device
        QString sqlqstr = "DELETE FROM images WHERE id = " + QString::number(id) + ";";

        QSqlQuery tmp = database.exec(sqlqstr);

        if (!tmp.exec())
            QMessageBox::critical(this, "Error", "Could not delete image with id = " + QString(i) + ". Error is: " + tmp.lastError().text(),
                QMessageBox::Ok);
    }

    updateImagesTableFromDb();

    QString statusMessage = "Deleted ";

    for(size_t i = 0; i < deletedImages.size(); i++)
    {
        if (i == deletedImages.size()-1 && i>0)
            statusMessage += "and ";

        statusMessage += deletedImages[i];

        if (i != deletedImages.size()-1 && deletedImages.size()>2)
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



    ui->label_status->setText("Opened image with id "+QString::number(id)+" from the images table in " + database.databaseName());



    std::string title = "Image ID: "+ std::to_string(id) + ", Capture Device ID: " + std::to_string(capDevice);
    cv::resize(img,img,cv::Size(1280,720));
    cv::imshow(title,img);
    cv::waitKey(0);
    cv::destroyAllWindows();
}


void manageTrainingDataForm::on_tableView_imagesForSegRegions_doubleClicked(const QModelIndex &index)
{
    int id = index.sibling(index.row(), 0).data().toInt();
    QByteArray bytes = index.sibling(index.row(), 1).data().toByteArray();
    //int capDevice = index.sibling(index.row(), 2).data().toInt();

    cv::Mat img = manageTrainingDataForm::byteArray2Mat(bytes);

    imgToDrawOn = img.clone();

    cv::cvtColor(img, img, CV_BGR2RGB);

    ui->label_status->setText("Opened image with id "+QString::number(id)+" from the images table in " + database.databaseName());

    ui->label_picToDrawOn->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));
}

void manageTrainingDataForm::on_btn_startContour_clicked()
{
    if (ui->radioBtn_selectOuterContour->isChecked())
    {
        currentDrawingMode = POLY_DRAWING_MODE::DRAWING_OUTER_CONTOUR;
        outerContour.clear();
    }
    else if (ui->radioBtn_selectInnerContour->isChecked())
    {
        currentDrawingMode = POLY_DRAWING_MODE::DRAWING_INNER_CONTOUR;
        std::vector<Point2<int>> tmp;
        innerContours.push_back(tmp);
    }
    ui->btn_startContour->setEnabled(false);
    ui->btn_endContour->setEnabled(true);

}

void manageTrainingDataForm::on_btn_endContour_clicked()
{

    ui->btn_startContour->setEnabled(true);
    ui->btn_endContour->setEnabled(false);

    if (outerContour.size()>0)
        outerContour.push_back(outerContour[0]);
    else if (innerContours.back().size() > 0)
        innerContours.back().push_back(innerContours.back()[0]);

    currentDrawingMode = POLY_DRAWING_MODE::NOT_DRAWING;
}

