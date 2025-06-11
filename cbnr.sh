#!/bin/bash

OS_TYPE="$(uname)"

if [ "$OS_TYPE" == "Linux" ]; then
    echo "Cleaning build directory..."
    rm -rf build
    rm -f ./oop_project

    echo "Creating build directory..."
    mkdir -p build
    cd build || exit 1

    echo "Running qmake..."
    qmake ../oop_project.pro
    if [ $? -ne 0 ]; then
        echo "!! qmake failed"
        exit 1
    fi

    echo "Compiling..."
    make -j$(nproc)
    if [ $? -ne 0 ]; then
        echo "!! Compilation failed"
        exit 2
    fi

    echo "Moving executable to root directory..."
    mv ./oop_project ../
    cd ..

    echo "Running app..."
    ./oop_project

elif [ "$OS_TYPE" == "Darwin" ]; then
    echo "macOS detected - eseguo comandi alternativi"

    echo "Specifica del path per qmake..."

    rm -rf build/
    rm -rf oop_project
    rm -rf oop_project.pro
    rm -rf .qmake.stash
    rm -rf Makefile

    echo "Rigenerazione del file .pro..."
    qmake -project -o oop_project.pro

    echo "Aggiunta moduli Qt e disattivazione del bundle..."
    echo -e "\nQT += core gui widgets xml\nCONFIG -= app_bundle" >> oop_project.pro
    echo "RESOURCES += view/resources.qrc" >> oop_project.pro
    echo "DESTDIR = ." >> oop_project.pro
    echo "OBJECTS_DIR = build" >> oop_project.pro
    echo "MOC_DIR = build" >> oop_project.pro
    echo "RCC_DIR = build" >> oop_project.pro
    echo "UI_DIR = build" >> oop_project.pro

    echo "Creating build directory..."
    mkdir -p build

    echo "Running qmake..."
    qmake oop_project.pro
    if [ $? -ne 0 ]; then
        echo "!! qmake failed"
        exit 1
    fi

    echo "Compiling..."
    make -j$(sysctl -n hw.ncpu)
    if [ $? -ne 0 ]; then
        echo "!! Compilation failed"
        exit 2
    fi

    echo "Running app..."
    ./oop_project

else
    echo "Sistema operativo non supportato: $OS_TYPE"
    exit 3
fi