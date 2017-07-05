#-------------------------------------------------
#
# Project created by QtCreator 2017-06-17T17:25:35
#
#-------------------------------------------------



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imageClassifier
TEMPLATE = app

QT       += core gui
QT += sql

CONFIG += c++11


DEFINES+=EnableUserPause=1

#add the various project subfolder to the includepath so that changes to project structure don't require lots of changes to include statements.
INCLUDEPATH +=  $$PWD/classifierAlgorithms\
                $$PWD/dataTypes\
                $$PWD/system\
                $$PWD/testing\
                $$PWD/gui\
                $$PWD/trainingDataManager\
                $$PWD/featureDetection

win32{

DEFINES+=Windows=1



INCLUDEPATH +=  C:/opencv_3_2_0/build/include\
                C:/sqlite3\
                C:/tinyxml2

#link against opencv, sqlite3, and tinyxml2

#opencv functions will cause exceptions if the debug/release option specified
#in qtcreator doesn't match the debug/release version of the library (which
#in turn links the proper dll)
debug: LIBS += C:/opencv_3_2_0/build/x64/vc14/lib/opencv_world320d.lib
release: INCLUDEPATH += C:/opencv_3_2_0/build/x64/vc14/lib/opencv_world320.lib

LIBS += C:/sqlite3/sqlite3.lib\
        C:/tinyxml2/tinyxml2.lib

#add the opencvdll location to the system path for building the program
#QMAKE_EXTRA_TARGETS += customtarget1
#customtarget1.target = dummy
#customtarget1.commands = set PATH=C:\opencv_3_2_0\build\x64\vc14\bin;$(PATH)
#PRE_TARGETDEPS += dummy


}

gcc*{
DEFINES+=Linux=1
DEFINES+=TX1=1      #comment out if not building on the TX1

LIBS += -L/usr/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lopencv_features2d
LIBS += -L/usr/lib/aarch64-linux-gnu
LIBS += -lsqlite3 -ltinyxml2
}

SOURCES += main.cpp\
        mainwindow.cpp \
    dataTypes/ImageSequence.cpp \
    dataTypes/Img.cpp \
    dataTypes/RgbHistogram.cpp \
    system/SystemCall.cpp \
    testing/testing.cpp \
    trainingDataManager/manageTrainingDataForm.cpp \
    gui/ClickableLabel.cpp \
    gui/ClassifyCameraStreamForm.cpp \
    dataTypes/CvQt.cpp \
    gui/TrainTextureClassifierForm.cpp \
    trainingDataManager/TrainingImageDbWrapper.cpp \
    classifierAlgorithms/TextureClassifier.cpp \
    dataTypes/ConverterMethods.cpp \
    dataTypes/ImageStatistics.cpp \
    gui/TestRosCommunicationForm.cpp \
    featureDetection/stop_marker_detection.cpp \
    featureDetection/generalFunc.cpp \
    classifierAlgorithms/ColourStatisticsAnalyzerRgb.cpp \
    dataTypes/HsvHistogram.cpp \
    classifierAlgorithms/CSA_RGB_WN_v1.cpp \
    classifierAlgorithms/CSA_RGB_shiftHisto.cpp \
    classifierAlgorithms/IMG_SVM.cpp \
    classifierAlgorithms/CSA_RGB_WN_v2.cpp

HEADERS  += mainwindow.h \
    dataTypes/Img.h \
    dataTypes/RgbHistogram.h \
    system/SystemCall.h \
    testing/testing.h \
    dataTypes/Point2.h \
    trainingDataManager/manageTrainingDataForm.h \
    gui/ClickableLabel.h \
    gui/ClassifyCameraStreamForm.h \
    dataTypes/CvQt.h \
    gui/TrainTextureClassifierForm.h \
    trainingDataManager/TrainingImageDbWrapper.h \
    dataTypes/ImageSequence.h \
    dataTypes/Contour.h \
    dataTypes/SegmentedRegion.h \
    dataTypes/SegmentedImage.h \
    classifierAlgorithms/TextureClassifier.h \
    dataTypes/ConverterMethods.h \
    dataTypes/ImageStatistics.h \
    gui/TestRosCommunicationForm.h \
    featureDetection/stop_marker_detection.h \
    featureDetection/generalFunc.h \
    classifierAlgorithms/ColourStatisticsAnalyzerRgb.h \
    dataTypes/HsvHistogram.h \
    classifierAlgorithms/CSA_RGB_WN_v1.h \
    classifierAlgorithms/CSA_RGB_shiftHisto.h \
    classifierAlgorithms/IMG_SVM.h \
    classifierAlgorithms/CSA_RGB_WN_v2.h

FORMS    += mainwindow.ui \
    trainingDataManager/manageTrainingDataForm.ui \
    gui/ClassifyCameraStreamForm.ui \
    gui/TrainTextureClassifierForm.ui \
    gui/TestRosCommunicationForm.ui

DISTFILES += \
    README.md
