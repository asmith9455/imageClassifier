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
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QPixMap>
#include <QGraphicsPixmapItem>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ClickableLabel.h>
#include <Point2.h>
#include <vector>
#include <QTreeWidgetItem>
#include <QPainter>
#include <QPaintDevice>
#include <QPen>

namespace Ui {
class manageTrainingDataForm;
}

class manageTrainingDataForm : public QDialog
{
    Q_OBJECT

public:

    enum POLY_DRAWING_MODE
    {
        NOT_DRAWING, DRAWING_OUTER_CONTOUR, DRAWING_INNER_CONTOUR
    };

    explicit manageTrainingDataForm(QWidget *parent = 0);
    ~manageTrainingDataForm();

private slots:
    void on_btn_createImgDb_clicked();

    void on_btn_addCaptureDevice_clicked();

    void on_btn_selectDatabase_clicked();

    void on_btn_deleteSelectedCaptureDevice_clicked();

    void on_btn_addProperty_clicked();

    void on_btn_deleteProperty_clicked();

    void on_btn_selectImageToAdd_clicked();

    void on_btn_addImgToDb_clicked();

    void on_tableView_images_doubleClicked(const QModelIndex &index);

    void on_btn_removeImgFromDb_clicked();

    void on_tableView_imagesForSegRegions_doubleClicked(const QModelIndex &index);

    void on_label_picToDrawOn_clicked(int mouseX, int mouseY);
    void on_btn_startContour_clicked();

    void on_btn_endContour_clicked();


    void on_btn_clearContours_clicked();


    void on_btn_addSegmentedRegionToDb_clicked();

    void on_btn_deleteSegmentedRegion_clicked();

    void on_tableView_segmentedRegions_doubleClicked(const QModelIndex &index);

private:
    Ui::manageTrainingDataForm *ui;
    static int nullCallback(void *NotUsed, int argc, char **argv, char **azColName);
    static QByteArray mat2ByteArray(const cv::Mat &image);
    static cv::Mat byteArray2Mat(const QByteArray & byteArray);


    void storeCaptureDevice(QString name, int imageWidth, int imageHeight, int bitsPerPixel);
    void storeProperty(QString propertyName);
    void storeImage(int captureDeviceReference, cv::Mat image);
    int storeSegmentedRegion(int imgRef, QString outerPoly, QString innerPoly);

    void updateAllTables();
    void updateCaptureDeviceTableFromDb();
    void updatePropertiesTableFromDb();
    void updateImagesTableFromDb();
    void updateSegmentedRegionsTableFromDb();
    void updatePropertiesForSegmentedRegionsTableFromDb();

    cv::Mat imgToStore;

    cv::Mat imgToDrawOn, imgToDrawOnRGB;
    int imgToDrawOnId = -1;
    std::vector<Point2<int>> outerContour;
    std::vector<std::vector<Point2<int>>> innerContours;
    POLY_DRAWING_MODE currentDrawingMode = POLY_DRAWING_MODE::NOT_DRAWING;
    void redrawSegmentedRegion();
    QTreeWidgetItem* outerContourParent;
    std::vector<QTreeWidgetItem> outerContourPoints;
    std::vector<QTreeWidgetItem> innerContourParents;
    QString getStringFromContour(std::vector<Point2<int>> vec);
    QString getStringFromContourVector(std::vector<std::vector<Point2<int>>> vec);
    std::vector<Point2<int>> getContourFromString(QString str);
    std::vector<std::vector<Point2<int>>> getContourVectorFromString(QString str);
    void manageTrainingDataForm::clearContours();

    QSqlDatabase database;
    bool dbOpened = false;
};

#endif // MANAGETRAININGDATAFORM_H
