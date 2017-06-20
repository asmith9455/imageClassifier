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

#add the various project subfolder to the includepath so that changes to project structure don't require lots of changes to include statements.
INCLUDEPATH +=  $$PWD/classifierAlgorithms\
                C:/opencv_3_2_0/build/include\
                C:/sqlite3\
                $$PWD/dataTypes\
                $$PWD/system\
                $$PWD/testing\
                $$PWD/gui\
                $$PWD/trainingDataManager\

#link against opencv
LIBS += C:/opencv_3_2_0/build/x64/vc14/lib/opencv_world320.lib\
        C:/sqlite3/sqlite3.lib


#add the opencvdll location to the system path for building the program
QMAKE_EXTRA_TARGETS += customtarget1
customtarget1.target = dummy
customtarget1.commands = set PATH=C:\opencv_3_2_0\build\x64\vc14\bin;$(PATH)
PRE_TARGETDEPS += dummy

SOURCES += main.cpp\
        mainwindow.cpp \
    classifierAlgorithms/ColourStatisticsAnalyzer.cpp \
    classifierAlgorithms/ImageClassifier.cpp \
    classifierAlgorithms/ImageSequence.cpp \
    dataTypes/Img.cpp \
    dataTypes/RgbHistogram.cpp \
    system/SystemCall.cpp \
    testing/testing.cpp \
    trainingDataManager/ImgDbManager.cpp \
    ProgramManager.cpp \
    manageTrainingDataForm.cpp \
    ClickableLabel.cpp \
    gui/TreeModel.cpp \
    gui/TreeItem.cpp

HEADERS  += mainwindow.h \
    classifierAlgorithms/ColourStatisticsAnalyzer.h \
    classifierAlgorithms/ImageClassifier.h \
    classifierAlgorithms/ImageSequence.h \
    dataTypes/Img.h \
    dataTypes/RgbHistogram.h \
    system/applicationDefines.h \
    system/SystemCall.h \
    testing/testing.h \
    trainingDataManager/ImgDbManager.h \
    ProgramManager.h \
    manageTrainingDataForm.h \
    ClickableLabel.h \
    dataTypes/Point2.h \
    gui/TreeModel.h \
    gui/TreeItem.h

FORMS    += mainwindow.ui \
    manageTrainingDataForm.ui

DISTFILES += \
    README.md
