TEMPLATE = app

TARGET = CQShaderToy

QT += widgets opengl

DEPENDPATH += .

MOC_DIR = .moc

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

QMAKE_CXXFLAGS += -std=c++17

INCLUDEPATH += \

SOURCES += \
CQShaderToy.cpp \

HEADERS += \
CQShaderToy.h \

unix:LIBS += \
