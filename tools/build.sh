#!/bin/bash

QT_PATH=$(./find_qt.py)

echo $QT_PATH

cd ..

mkdir build
cd build

$QT_PATH/bin/qmake ../src/GitIssue.pro
make -j

cd ..
rm -rf build
