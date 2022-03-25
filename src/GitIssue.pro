TEMPLATE = app
TARGET = GitIssue

QT -= gui
QT += network

CONFIG += c++17

DESTDIR = $$PWD/../bin
macos:CONFIG -= app_bundle

HEADERS += \
   Credentials.h \
   Exception.h \
   Issue.h \
   SessionAbstract.h \
   SessionGitHub.h \
   SessionGitLab.h \
   Settings.h

SOURCES += \
   Credentials.cpp \
   Exception.cpp \
   Issue.cpp \
   SessionAbstract.cpp \
   SessionGitHub.cpp \
   SessionGitLab.cpp \
   Settings.cpp \
   main.cpp
