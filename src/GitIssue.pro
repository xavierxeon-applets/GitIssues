TEMPLATE = app
TARGET = GitIssue

QT -= gui
QT += network

CONFIG += c++17

DESTDIR = $$PWD/../bin
macos:CONFIG -= app_bundle

HEADERS += \
   Credentials.h \
   Encryption.h \
   Exception.h \
   GitIssue.h \
   SessionAbstract.h \
   SessionGitHub.h \
   SessionGitLab.h \
   Settings.h

SOURCES += \
   Credentials.cpp \
   Encryption.cpp \
   Exception.cpp \
   GitIssue.cpp \
   SessionAbstract.cpp \
   SessionGitHub.cpp \
   SessionGitLab.cpp \
   Settings.cpp
