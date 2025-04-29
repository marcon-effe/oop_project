#!/bin/bash

OS_TYPE="$(uname)"

if [ "$OS_TYPE" == "Linux" ]; then
    echo "🔧 Linux build incrementale..."

    mkdir -p build
    cd build || exit 1

    echo "🔧 qmake..."
    qmake ../oop_project.pro
    if [ $? -ne 0 ]; then
        echo "❌ qmake failed"
        exit 1
    fi

    echo "⚙️ make..."
    make -j$(nproc)
    if [ $? -ne 0 ]; then
        echo "❌ Compilation failed"
        exit 2
    fi

    echo "🚀 Running app..."
    cd ..
    ./oop_project

elif [ "$OS_TYPE" == "Darwin" ]; then
    echo "🍎 macOS build incrementale..."

    QMAKE_PATH="/Users/leonardo/Qt/6.8.2/macos/bin/qmake"

    mkdir -p build

    echo "🔧 qmake..."
    "$QMAKE_PATH" oop_project.pro
    if [ $? -ne 0 ]; then
        echo "❌ qmake failed"
        exit 1
    fi

    echo "⚙️ make..."
    make -j$(sysctl -n hw.ncpu)
    if [ $? -ne 0 ]; then
        echo "❌ Compilation failed"
        exit 2
    fi

    echo "🚀 Running app..."
    ./oop_project

else
    echo "⚠️ Sistema operativo non supportato: $OS_TYPE"
    exit 3
fi