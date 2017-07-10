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
#include <ColourStatisticsAnalyzerRgb.h>
#include <TextureClassifier.h>
#include <CSA_RGB_shiftHisto.h>
#include <CSA_RGB_WN_v1.h>
#include <IMG_SVM.h>
#include <ClassifyCameraStreamForm.h>

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

    void on_btn_saveTrainingData_clicked();

    void on_btn_testClassifier_clicked();

    void on_btn_trainFromGeneratedImages_clicked();

    void on_btn_trainFromFile_clicked();

    void on_btn_openImageClassifier_clicked();

private:

    bool usePreClustering = false;

    Ui::TrainClassifierForm *ui;
    void updatePropertiesFromDb();
    void updateCaptureDevicesFromDb();
    QString dbPath;

    //images and image counts
    std::vector<ImageSequence>  targetImgsForTraining, targetImgsNotForTraining;
    std::vector<ImageSequence>  nonTargetImgsForTraining, nonTargetImgsNotForTraining;
    ImageSequence  targetImgsForTrainingNPC, targetImgsNotForTrainingNPC;
    ImageSequence  nonTargetImgsForTrainingNPC, nonTargetImgsNotForTrainingNPC;
    size_t numTargetImgsForTraining = 0, numTargetImgsNotForTraining = 0;
    size_t numNonTargetImgsForTraining = 0, numNonTargetImgsNotForTraining = 0;
    std::shared_ptr<TextureClassifier> ic;

    std::vector<std::shared_ptr<ClassifyCameraStreamForm>> classifyForms;

};

#endif // TRAINCLASSIFIERFORM_H
