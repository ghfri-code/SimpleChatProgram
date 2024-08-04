TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
#LIBS += D:\WorkSpace\C\textFileChat\chat\libws2_32.a
#LIBS += -lws2_32
LIBS +=  -lws2_32
SOURCES += \
        main.c

HEADERS += \
    Parameters.h

QMAKE_CXXFLAGS += -Wno-padded

