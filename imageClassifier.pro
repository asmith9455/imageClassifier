#-------------------------------------------------
#
# Project created by QtCreator 2017-06-17T17:25:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imageClassifier
TEMPLATE = app


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
    ProgramManager.cpp

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
    ProgramManager.h

FORMS    += mainwindow.ui

DISTFILES += \
    README.md
