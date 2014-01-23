HEADERS += \
    CommonFunctions.h \
    ModInfos.h \
    SaveGame.h \
    mainwindow.h \
    savegameinfos.h \
    qtreecustom.h

SOURCES += \
    CommonFunctions.cpp \
    main.cpp \
    ModInfos.cpp \
    SaveGame.cpp \
    mainwindow.cpp \
    savegameinfos.cpp \
    qtreecustom.cpp

QT += widgets
CONFIG += console


QMAKE_CXXFLAGS += -std=c++11
#QMAKE_CXXFLAGS += -g -fno-omit-frame-pointer
#QMAKE_CXXFLAGS += -gdwarf-2
QMAKE_CXXFLAGS += -static
