#include "TrainTextureClassifierForm.h"
#include "ui_TrainTextureClassifierForm.h"


TrainClassifierForm::TrainClassifierForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrainClassifierForm)
{
    ui->setupUi(this);

    ui->comboBox_img_svm_grey_rgb->addItem("RGB");
    ui->comboBox_img_svm_grey_rgb->addItem("GREY");

    ui->comboBox_img_svm_svmType->addItem("C_SVC");
    ui->comboBox_img_svm_svmType->addItem("NU_SVC");
    ui->comboBox_img_svm_svmType->addItem("EPS_SVR");
    ui->comboBox_img_svm_svmType->addItem("NU_SVR");

    ui->comboBox_hog_svm_svmType->addItem("C_SVC");
    ui->comboBox_hog_svm_svmType->addItem("NU_SVC");
    ui->comboBox_hog_svm_svmType->addItem("EPS_SVR");
    ui->comboBox_hog_svm_svmType->addItem("NU_SVR");

    ui->comboBox_hog_svm_kernelType->addItem("LINEAR");
    ui->comboBox_hog_svm_kernelType->addItem("POLY");
    ui->comboBox_hog_svm_kernelType->addItem("RBF");
    ui->comboBox_hog_svm_kernelType->addItem("SIGMOID");
    ui->comboBox_hog_svm_kernelType->addItem("CHI2");
    ui->comboBox_hog_svm_kernelType->addItem("INTER");

    dbPath = QFileDialog::getOpenFileName(this, tr("Select Image Database"),
                               "C:/",
                               tr("(*.sqlite)"));
    updateCaptureDevicesFromDb();
    updatePropertiesFromDb();
}

TrainClassifierForm::~TrainClassifierForm()
{
    delete ui;
}

void TrainClassifierForm::updateCaptureDevicesFromDb()
{

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(dbPath);

    if (!database.open())
    {
        QMessageBox::critical(this, "Error", "Could not open a connection to the database.",
            QMessageBox::Ok);
        return;
    }

    database.exec("PRAGMA foreign_keys = ON;");

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

    database.close();
}

void TrainClassifierForm::updatePropertiesFromDb()
{



    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(dbPath);

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
    ui->tableView_propertiesTarget->setModel(model);
    ui->tableView_propertiesNonTarget->setModel(model);
    database.close();
}

void TrainClassifierForm::on_btn_generateImages_clicked()
{
    targetImgsForTraining.clear();
    targetImgsNotForTraining.clear();
    nonTargetImgsForTraining.clear();
    nonTargetImgsNotForTraining.clear();

    targetImgsForTrainingNPC.clear();
    targetImgsNotForTrainingNPC.clear();
    nonTargetImgsForTrainingNPC.clear();
    nonTargetImgsNotForTrainingNPC.clear();

    //get the currently selected capture device
    QItemSelectionModel *select = ui->tableView_captureDevices->selectionModel();
    if (select == NULL)
    {
        QMessageBox::information(this, "Information", "Please select a captureDevice.",
            QMessageBox::Ok);
        return;
    }
    if (!select->hasSelection())
    {
        QMessageBox::information(this, "Information", "Please select a captureDevice.",
            QMessageBox::Ok);
        return;
    }
    QModelIndex index = select->selectedRows().at(0); //get the first selected capture device
    int captureDeviceID = index.sibling(index.row(), 0).data().toInt();





    //get the currently selected propertyTarget
    QItemSelectionModel *select2 = ui->tableView_propertiesTarget->selectionModel();
    if (select == NULL)
    {
        QMessageBox::information(this, "Information", "Please select a property (target).",
            QMessageBox::Ok);
        return;
    }
    if (!select2->hasSelection())
    {
        QMessageBox::information(this, "Information", "Please select a property (target).",
            QMessageBox::Ok);
        return;
    }
    QModelIndex index2 = select2->selectedRows().at(0); //get the first selected property
    int propertyTargetID = index2.sibling(index2.row(), 0).data().toInt();





    //get the currently selected propertyNonTarget
    QItemSelectionModel *select3 = ui->tableView_propertiesNonTarget->selectionModel();
    if (select == NULL)
    {
        QMessageBox::information(this, "Information", "Please select a property (non-target).",
            QMessageBox::Ok);
        return;
    }
    if (!select3->hasSelection())
    {
        QMessageBox::information(this, "Information", "Please select a property  (non-target).",
            QMessageBox::Ok);
        return;
    }
    QModelIndex index3 = select3->selectedRows().at(0); //get the first selected property
    int propertyNonTargetID = index3.sibling(index3.row(), 0).data().toInt();

    std::vector<ImageSequence> targetImgs, nonTargetImgs;
    ImageSequence targetImgsNPC, nonTargetImgsNPC;
    usePreClustering = ui->checkBox_usePreClustering->isChecked();
    if (usePreClustering)
    {
        //generate pre-clustered image sequences
        targetImgs = TrainingImageDbWrapper::getPreClusteredTilesWithSingleProperty(
                    dbPath,
                    captureDeviceID,
                    propertyTargetID,
                    ui->lineEdit_tileWidth->text().toInt(),
                    ui->lineEdit_tileHeight->text().toInt()
                    );

        nonTargetImgs = TrainingImageDbWrapper::getPreClusteredTilesWithSingleProperty(
                    dbPath,
                    captureDeviceID,
                    propertyNonTargetID,
                    ui->lineEdit_tileWidth->text().toInt(),
                    ui->lineEdit_tileHeight->text().toInt()
                    );
    }
    else
    {
        targetImgsNPC = TrainingImageDbWrapper::getTilesWithSingleProperty(
                    dbPath,
                    captureDeviceID,
                    propertyTargetID,
                    ui->lineEdit_tileWidth->text().toInt(),
                    ui->lineEdit_tileHeight->text().toInt()
                    );

        nonTargetImgsNPC = TrainingImageDbWrapper::getTilesWithSingleProperty(
                    dbPath,
                    captureDeviceID,
                    propertyNonTargetID,
                    ui->lineEdit_tileWidth->text().toInt(),
                    ui->lineEdit_tileHeight->text().toInt()
                    );
    }

    float trainingImagesFraction = ui->lineEdit_trainingImageFraction->text().toFloat();
    unsigned int randSeed = ui->lineEdit_rngSeed->text().toUInt();
    bool useRandSeed = ui->checkBox_useRngSeed->isChecked();


    std::pair<ImageSequence, ImageSequence> tmpPair;



    if (usePreClustering)
    {
        for (ImageSequence is : targetImgs)
        {
            tmpPair = ImageSequence::getRandomImageSequence(is, trainingImagesFraction, useRandSeed, randSeed);
            targetImgsForTraining.push_back(tmpPair.first);
            targetImgsNotForTraining.push_back(tmpPair.second);
        }

        for (ImageSequence is : nonTargetImgs)
        {
            tmpPair = ImageSequence::getRandomImageSequence(is, trainingImagesFraction, useRandSeed, randSeed);
            nonTargetImgsForTraining.push_back(tmpPair.first);
            nonTargetImgsNotForTraining.push_back(tmpPair.second);
        }
    }
    else
    {
        tmpPair = ImageSequence::getRandomImageSequence(targetImgsNPC, trainingImagesFraction, useRandSeed, randSeed);
        targetImgsForTrainingNPC = tmpPair.first;
        targetImgsNotForTrainingNPC = tmpPair.second;

        tmpPair = ImageSequence::getRandomImageSequence(nonTargetImgsNPC, trainingImagesFraction, useRandSeed, randSeed);
        nonTargetImgsForTrainingNPC = tmpPair.first;
        nonTargetImgsNotForTrainingNPC = tmpPair.second;
    }

    numTargetImgsForTraining = 0;
    numTargetImgsNotForTraining = 0;
    numNonTargetImgsForTraining = 0;
    numNonTargetImgsNotForTraining = 0;

    if (usePreClustering)
    {
        for (size_t i = 0; i < targetImgsForTraining.size(); i++)
        {
            numTargetImgsForTraining += targetImgsForTraining[i].getimageCount();
            numTargetImgsNotForTraining += targetImgsNotForTraining[i].getimageCount();
        }

        for (size_t i = 0; i < nonTargetImgsForTraining.size(); i++)
        {
            numNonTargetImgsForTraining += nonTargetImgsForTraining[i].getimageCount();
            numNonTargetImgsNotForTraining += nonTargetImgsNotForTraining[i].getimageCount();
        }
    }
    else
    {
        numTargetImgsForTraining =targetImgsForTrainingNPC.getimageCount();
        numTargetImgsNotForTraining = targetImgsNotForTrainingNPC.getimageCount();
        numNonTargetImgsForTraining = nonTargetImgsForTrainingNPC.getimageCount();
        numNonTargetImgsNotForTraining = nonTargetImgsNotForTrainingNPC.getimageCount();
    }

    ui->label_numTargetTiles->setText("Number of Target Tiles (Total): " + QString::number(numTargetImgsForTraining + numTargetImgsNotForTraining));
    ui->label_numNonTargetTiles->setText("Number of nonTarget Tiles (Total): " + QString::number(numNonTargetImgsForTraining + numNonTargetImgsNotForTraining));
    ui->label_numTargetTilesTraining->setText("Number of Target Tiles (Training): " + QString::number(numTargetImgsForTraining));
    ui->label_numNonTargetTilesTraining->setText("Number of nonTarget Tiles (Training): " + QString::number(numNonTargetImgsForTraining));

}

void TrainClassifierForm::on_btn_saveTrainingData_clicked()
{
    QString toSavePath = QFileDialog::getSaveFileName(this, tr("Choose the save location."),
                               "C:/",
                               tr("(*.xml)"));

    ic->writeToFile(toSavePath.toStdString());
}

void TrainClassifierForm::on_btn_testClassifier_clicked()
{

    bool r;

    size_t numTruePositivesForTrainedImages = 0;
    size_t numTruePositivesForUntrainedImages = 0;
    size_t numFalsePositivesForTrainedImages = 0;
    size_t numFalsePositivesForUntrainedImages = 0;

    if (usePreClustering)
    {
        for (size_t i = 0; i < targetImgsForTraining.size(); i++)
            for (size_t j = 0; j < targetImgsForTraining[i].getimageCount(); j++)
            {
                r = ic->isTarget(targetImgsForTraining[i].imageAt(j));

                if (r)
                    numTruePositivesForTrainedImages++;
            }

        for (size_t i = 0; i < targetImgsNotForTraining.size(); i++)
            for (size_t j = 0; j < targetImgsNotForTraining[i].getimageCount(); j++)
            {
                r = ic->isTarget(targetImgsNotForTraining[i].imageAt(j));

                if (r)
                    numTruePositivesForUntrainedImages++;
            }


        for (size_t i = 0; i < nonTargetImgsForTraining.size(); i++)
            for (size_t j = 0; j < nonTargetImgsForTraining[i].getimageCount(); j++)
            {
                r = ic->isTarget(nonTargetImgsForTraining[i].imageAt(j));

                if (r)
                    numFalsePositivesForTrainedImages++;
            }


        for (size_t i = 0; i < nonTargetImgsNotForTraining.size(); i++)
            for (size_t j = 0; j < nonTargetImgsNotForTraining[i].getimageCount(); j++)
            {
                r = ic->isTarget(nonTargetImgsNotForTraining[i].imageAt(j));

                if (r)
                    numFalsePositivesForUntrainedImages++;
            }
    }
    else
    {

        for (size_t j = 0; j < targetImgsForTrainingNPC.getimageCount(); j++)
        {
            r = ic->isTarget(targetImgsForTrainingNPC.imageAt(j));

            if (r)
                numTruePositivesForTrainedImages++;
        }

        for (size_t j = 0; j < targetImgsNotForTrainingNPC.getimageCount(); j++)
        {
            r = ic->isTarget(targetImgsNotForTrainingNPC.imageAt(j));

            if (r)
                numTruePositivesForUntrainedImages++;
        }

        for (size_t j = 0; j < nonTargetImgsForTrainingNPC.getimageCount(); j++)
        {
            r = ic->isTarget(nonTargetImgsForTrainingNPC.imageAt(j));

            if (r)
                numFalsePositivesForTrainedImages++;
        }

        for (size_t j = 0; j < nonTargetImgsNotForTrainingNPC.getimageCount(); j++)
        {
            r = ic->isTarget(nonTargetImgsNotForTrainingNPC.imageAt(j));

            if (r)
                numFalsePositivesForUntrainedImages++;
        }
    }

    size_t numTruePositivesTotal =(numTruePositivesForTrainedImages+numTruePositivesForUntrainedImages);
    size_t numTargetImgsTotal = (numTargetImgsForTraining+numTargetImgsNotForTraining);

    size_t numFalsePositivesTotal = (numFalsePositivesForTrainedImages+numFalsePositivesForUntrainedImages);
    size_t numNonTargetImgsTotal = (numNonTargetImgsForTraining+numNonTargetImgsNotForTraining);


    double truePositivesTrainingRate =
            (double) numTruePositivesForTrainedImages
            / (double) numTargetImgsForTraining;

    double falsePositivesTrainingRate =
            (double) (numFalsePositivesForTrainedImages)
            / (double) (numNonTargetImgsForTraining);

    double successRateTraining =
            (double) (numTruePositivesForTrainedImages + numNonTargetImgsForTraining - numFalsePositivesForTrainedImages)
            / (double) (numTargetImgsForTraining + numNonTargetImgsForTraining);

    double truePositivesUntrainedRate =
            (double) numTruePositivesForUntrainedImages
            / (double) numTargetImgsNotForTraining;

    double falsePositivesUntrainedRate =
            (double) numFalsePositivesForUntrainedImages
            / (double) numNonTargetImgsNotForTraining;

    double successRateUntrained =
            (double) (numTruePositivesForUntrainedImages + numNonTargetImgsNotForTraining - numFalsePositivesForUntrainedImages)
            / (double) (numTargetImgsNotForTraining + numNonTargetImgsNotForTraining);



    double truePositivesOverallRate
            = (double) numTruePositivesTotal
            / (double) numTargetImgsTotal;


    double falsePositivesOverallRate
            = (double) numFalsePositivesTotal
            / (double) numNonTargetImgsTotal;

    double successRate
            = (double)(numTruePositivesTotal+numNonTargetImgsTotal-numFalsePositivesTotal)
            / (double)(numTargetImgsTotal + numNonTargetImgsTotal);

    ui->label_truePositivesRateTrained->setText("True Positives: " + QString::number(truePositivesTrainingRate*100) + "%");
    ui->label_falsePositivesRateTrained->setText("False Positives: " + QString::number(falsePositivesTrainingRate*100) + "%");
    ui->label_successRateTrained->setText("Success Rate: " + QString::number(successRateTraining*100) + "%");

    ui->label_truePositivesRateUntrained->setText("True Positives: " + QString::number(truePositivesUntrainedRate*100) + "%");
    ui->label_falsePositivesRateUntrained->setText("False Positives: " + QString::number(falsePositivesUntrainedRate*100) + "%");
    ui->label_successRateUntrained->setText("Success Rate: " + QString::number(successRateUntrained*100) + "%");

    ui->label_truePositivesTotal->setText("True Positives: " + QString::number(truePositivesOverallRate*100.0) + "%");
    ui->label_falsePositivesTotal->setText("False Positives: " + QString::number(falsePositivesOverallRate*100.0) + "%");
    ui->label_successRateTotal->setText("Success Rate: " + QString::number(successRate*100.0) + "%");
}

void TrainClassifierForm::on_btn_trainFromGeneratedImages_clicked()
{

    double safetyFactor = ui->lineEdit_safetyFactor->text().toDouble();
    usePreClustering = ui->checkBox_usePreClustering->isChecked();

    if (usePreClustering && ui->radioButton_useColourStatisticsAnalyzer->isChecked())
    {
        std::shared_ptr<ColourStatisticsAnalyzerRgb> csa
                = std::make_shared<ColourStatisticsAnalyzerRgb>
                (ColourStatisticsAnalyzerRgb(targetImgsForTraining, nonTargetImgsForTraining, safetyFactor));

        ic = static_pointer_cast<TextureClassifier>(csa);
    }

    else if (!usePreClustering && ui->radioButton_useColourStatisticsAnalyzer->isChecked())
    {
        std::shared_ptr<ColourStatisticsAnalyzerRgb> csa
                = std::make_shared<ColourStatisticsAnalyzerRgb>
                (ColourStatisticsAnalyzerRgb(targetImgsForTrainingNPC, nonTargetImgsForTrainingNPC, safetyFactor));

        ic = static_pointer_cast<TextureClassifier>(csa);
    }

    else if (!usePreClustering && ui->radioButton_useCSA_RGB_WN_v1->isChecked())
    {
        double safetyFactor2 = ui->lineEdit_safetyFactor2->text().toDouble();

        std::shared_ptr<CSA_RGB_WN_v1> tmp
                = std::make_shared<CSA_RGB_WN_v1>
                (CSA_RGB_WN_v1(targetImgsForTrainingNPC, nonTargetImgsForTrainingNPC, safetyFactor2));

        ic = static_pointer_cast<TextureClassifier>(tmp);
    }
    else if (!usePreClustering && ui->radioButton_useCSA_RGB_shiftHisto->isChecked())
    {
        double safetyFactor2 = ui->lineEdit_safetyFactor2->text().toDouble();

        std::shared_ptr<CSA_RGB_shiftHisto> tmp
                = std::make_shared<CSA_RGB_shiftHisto>
                (CSA_RGB_shiftHisto(targetImgsForTrainingNPC, nonTargetImgsForTrainingNPC, safetyFactor2));

        ic = static_pointer_cast<TextureClassifier>(tmp);
    }
    else if (!usePreClustering && ui->radioButton_useIMG_SVM->isChecked())
    {
        IMG_SVM::ColourMode cmode;
        if (ui->comboBox_img_svm_grey_rgb->currentText() == "GREY")
            cmode = IMG_SVM::ColourMode::GREY;
        else if (ui->comboBox_img_svm_grey_rgb->currentText() == "RGB")
            cmode = IMG_SVM::ColourMode::RGB;


        std::shared_ptr<IMG_SVM> tmp
                = std::make_shared<IMG_SVM>
                (IMG_SVM(targetImgsForTrainingNPC, nonTargetImgsForTrainingNPC, cmode));

        ic = static_pointer_cast<TextureClassifier>(tmp);
    }
    else
    {
        QMessageBox::critical(this, "Error", "Pre-clustering mode not supported. ",
            QMessageBox::Ok);
    }

    ic->analyze();
}

void TrainClassifierForm::on_btn_trainFromFile_clicked()
{
    QString toReadPath = QFileDialog::getOpenFileName(this, tr("Select the training data to load."),
                               "C:/",
                               tr("(*.xml)"));

    if (toReadPath == "")
        return;

    try
    {
        ic->readFromFile(toReadPath.toStdString());
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(this, "Error", "Error reading training data. Error message is: "+QString(e.what()),
            QMessageBox::Ok);
        std::cerr << e.what() << std::endl;
    }

}

void TrainClassifierForm::on_btn_openImageClassifier_clicked()
{

    std::shared_ptr<ClassifyCameraStreamForm> tmp = std::make_shared<ClassifyCameraStreamForm>();
    tmp->setImageClassifier(ic);
    classifyForms.push_back(tmp);
    tmp->show();
}
