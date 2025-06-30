QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#不带cuda
INCLUDEPATH += D:\openCV\build-opencv-4.10.0-MSVC2019-Debug\install\include
LIBS += D:\openCV\build-opencv-4.10.0-MSVC2019-Debug\install\x64\vc17\lib\opencv_*


SOURCES += \
    m_graphicsview.cpp \
    main.cpp \
    mainwindow.cpp \
    sudoku.cpp

HEADERS += \
    m_graphicsview.h \
    mainwindow.h \
    sudoku.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
