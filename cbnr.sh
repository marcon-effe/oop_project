#!/bin/bash

echo "🧹 Cleaning build directory..."
rm -rf build
rm -rf ./oop_project

echo "📁 Creating build directory..."
mkdir -p build
cp -r saves build/saves
cd build

echo "🔧 Running qmake..."
qmake ../oop_project.pro

if [ $? -ne 0 ]; then
    echo "❌ qmake failed"
    exit 1
fi

echo "⚙️ Compiling..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "❌ Compilation failed"
    exit 2
fi

echo "🚀 Running app..."
./oop_project
