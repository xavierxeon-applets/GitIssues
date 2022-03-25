TEMPLATE = app
TARGET = GitIssue

QT -= gui
QT += network

CONFIG += c++17

DESTDIR = $$PWD/../bin
macos:CONFIG -= app_bundle

HEADERS += \
   SessionAbstract.h \
   SessionGitHub.h \
   SessionGitLab.h

SOURCES += \
   SessionAbstract.cpp \
   SessionGitHub.cpp \
   SessionGitLab.cpp \
   main.cpp
