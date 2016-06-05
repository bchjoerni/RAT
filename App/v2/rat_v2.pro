#-------------------------------------------------
#
# Project created by QtCreator 2015-06-05T15:30:58
#
#-------------------------------------------------

QT       += core gui bluetooth
QT       += androidextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rat_v2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \    
    bluetooth.cpp \
    devicesearch.cpp \
    graph.cpp \    
    result.cpp \
    ratanalyzer.cpp \
    currentanalyzer.cpp \
    finalanalyzer.cpp \
    graphcompression.cpp \
    graphventilation.cpp \
    graphscale.cpp \
    settings.cpp

HEADERS  += mainwindow.h \    
    bluetooth.h \
    datapoint.h \
    devicesearch.h \
    graph.h \   
    result.h \
    ratanalyzer.h \
    currentanalyzer.h \
    finalanalyzer.h \
    graphcompression.h \
    graphventilation.h \
    graphscale.h \
    settings.h

CONFIG += mobility c++11
MOBILITY = 

FORMS += \
    devicesearch.ui \
    main_landscape.ui \
    main_portrait.ui \
    result.ui \
    settings.ui

DISTFILES += \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

RESOURCES += \
    res.qrc

