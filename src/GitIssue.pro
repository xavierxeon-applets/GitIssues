TEMPLATE = app
TARGET = GitIssue

QT -= gui
QT += network

CONFIG += c++17

DESTDIR = $$PWD/../bin
macos:CONFIG -= app_bundle

HEADERS += \
   Exception.h \
   GitIssue.h \
   SessionAbstract.h \
   SessionGitHub.h \
   SessionGitLab.h

SOURCES += \
   Exception.cpp \
   GitIssue.cpp \
   SessionAbstract.cpp \
   SessionGitHub.cpp \
   SessionGitLab.cpp
