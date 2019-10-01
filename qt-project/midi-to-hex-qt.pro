#-------------------------------------------------
#
# Project created by QtCreator 2019-09-30T10:31:41
#
#-------------------------------------------------
QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = midi-to-hex-qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        ByteStream.cpp \
        NoteListProcessor.cpp \
        bprinter/src/table_printer.cpp \
        fmt/src/format.cc \
        main.cpp \
        mainwindow.cpp \
        masterthread.cpp \
        midifile/src/Binasc.cpp \
        midifile/src/MidiEvent.cpp \
        midifile/src/MidiEventList.cpp \
        midifile/src/MidiFile.cpp \
        midifile/src/MidiMessage.cpp \
        midifile/src/Options.cpp

HEADERS += \
        ByteStream.h \
        NoteListProcessor.h \
        bprinter/include/bprinter/impl/table_printer.tpp.h \
        bprinter/include/bprinter/table_printer.h \
        mainwindow.h \
        masterthread.h \
        midifile/include/Binasc.h \
        midifile/include/MidiEvent.h \
        midifile/include/MidiEventList.h \
        midifile/include/MidiFile.h \
        midifile/include/MidiMessage.h \
        midifile/include/Options.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += $$PWD/midifile/include \
               $$PWD/bprinter/include \
               $$PWD/fmt/include \

