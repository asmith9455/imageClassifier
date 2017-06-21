#-------------------------------------------------
#
# Project created by QtCreator 2017-06-17T17:25:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imageClassifier
TEMPLATE = app

QT += sql

CONFIG += c++11


DEFINES+=EnableUserPause=1

#add the various project subfolder to the includepath so that changes to project structure don't require lots of changes to include statements.
INCLUDEPATH +=  $$PWD/classifierAlgorithms\
                $$PWD/dataTypes\
                $$PWD/system\
                $$PWD/testing\
                $$PWD/gui\
                $$PWD/trainingDataManager

win32

{


DEFINES+=Windows=1

INCLUDEPATH +=  C:/opencv_3_2_0/build/include\
                C:/sqlite3

#link against opencv
LIBS += C:/opencv_3_2_0/build/x64/vc14/lib/opencv_world320.lib\
        C:/sqlite3/sqlite3.lib


#add the opencvdll location to the system path for building the program
QMAKE_EXTRA_TARGETS += customtarget1
customtarget1.target = dummy
customtarget1.commands = set PATH=C:\opencv_3_2_0\build\x64\vc14\bin;$(PATH)
PRE_TARGETDEPS += dummy


}

gcc*{
DEFINES+=Linux=1
DEFINES+=TX1=1      #comment out if not building on the TX1

LIBS += -L/usr/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lopencv_videoio

LIBS += -lsqlite3
}
SOURCES += main.cpp\
        mainwindow.cpp \
    classifierAlgorithms/ColourStatisticsAnalyzer.cpp \
    dataTypes/ImageSequence.cpp \
    dataTypes/Img.cpp \
    dataTypes/RgbHistogram.cpp \
    system/SystemCall.cpp \
    testing/testing.cpp \
    ProgramManager.cpp \
    trainingDataManager/manageTrainingDataForm.cpp \
    gui/ClickableLabel.cpp \
    gui/ClassifyCameraStreamForm.cpp \
    dataTypes/CvQt.cpp \
    gui/TrainTextureClassifierForm.cpp \
    trainingDataManager/TrainingImageDbWrapper.cpp \
    dataTypes/ImageSequence.cpp \
    classifierAlgorithms/TextureClassifier.cpp

HEADERS  += mainwindow.h \
    classifierAlgorithms/ColourStatisticsAnalyzer.h \
    dataTypes/Img.h \
    dataTypes/RgbHistogram.h \
    system/SystemCall.h \
    testing/testing.h \
    ProgramManager.h \
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
    classifierAlgorithms/TextureClassifier.h

FORMS    += mainwindow.ui \
    trainingDataManager/manageTrainingDataForm.ui \
    gui/ClassifyCameraStreamForm.ui \
    gui/TrainTextureClassifierForm.ui

DISTFILES += \
    README.md
