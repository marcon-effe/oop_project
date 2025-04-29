#!/bin/bash

echo "🧹 Pulizia della build..."

# Cancella directory build e tutti gli output della compilazione
rm -rf build
rm -f Makefile
rm -f *.o
rm -f *.obj
rm -f *.moc
rm -f *.user
rm -f moc_*.cpp
rm -f ui_*.h
rm -f qrc_*.cpp
rm -f ./oop_project

echo "✅ Build pulita con successo."