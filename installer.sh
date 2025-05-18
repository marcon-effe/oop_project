#!/bin/bash

OS_TYPE="$(uname)"
QMAKE_OVERRIDE="$1"

# Controllo validatore XML
if ! command -v xmllint &>/dev/null; then
    echo "Errore: 'xmllint' non è installato. Installa libxml2 (es: 'sudo apt install libxml2-utils' o 'brew install libxml2')."
    exit 1
fi

# Test supporto --schema
TMP_XML="__tmp_check__.xml"
TMP_XSD="__tmp_check__.xsd"

cat <<EOF > "$TMP_XML"
<?xml version="1.0"?><root/>
EOF

cat <<EOF > "$TMP_XSD"
<?xml version="1.0"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:element name="root" type="xs:string"/>
</xs:schema>
EOF

if ! xmllint --noout --schema "$TMP_XSD" "$TMP_XML" &>/dev/null; then
    echo "Errore: 'xmllint' è installato ma non supporta '--schema'."
    echo "Verifica la tua installazione di libxml2."
    rm -f "$TMP_XML" "$TMP_XSD"
    exit 1
fi

rm -f "$TMP_XML" "$TMP_XSD"

# Comando qmake (override se fornito)
if [ -n "$QMAKE_OVERRIDE" ]; then
    QMAKE="$QMAKE_OVERRIDE"
else
    QMAKE="qmake"
fi

# Build process
if [ "$OS_TYPE" == "Linux" ]; then
    rm -rf build oop_project

    mkdir -p build
    cd build || exit 1

    $QMAKE ../oop_project.pro
    if [ $? -ne 0 ]; then
        echo "Errore: qmake fallito"
        exit 1
    fi

    make -j$(nproc)
    if [ $? -ne 0 ]; then
        echo "Errore: compilazione fallita"
        exit 2
    fi

    mv ./oop_project ../
    cd ..
    ./oop_project

elif [ "$OS_TYPE" == "Darwin" ]; then
    rm -rf build/ oop_project oop_project.pro .qmake.stash Makefile

    $QMAKE -project -o oop_project.pro
    if [ $? -ne 0 ]; then
        echo "Errore: qmake -project fallito"
        exit 1
    fi

    echo -e "\nQT += core gui widgets xml\nCONFIG -= app_bundle" >> oop_project.pro
    echo "RESOURCES += view/resources.qrc" >> oop_project.pro
    echo "DESTDIR = ." >> oop_project.pro
    echo "OBJECTS_DIR = build" >> oop_project.pro
    echo "MOC_DIR = build" >> oop_project.pro
    echo "RCC_DIR = build" >> oop_project.pro
    echo "UI_DIR = build" >> oop_project.pro

    mkdir -p build

    $QMAKE oop_project.pro
    if [ $? -ne 0 ]; then
        echo "Errore: qmake fallito"
        exit 1
    fi

    make -j$(sysctl -n hw.ncpu)
    if [ $? -ne 0 ]; then
        echo "Errore: compilazione fallita"
        exit 2
    fi

    ./oop_project

else
    echo "Sistema operativo non supportato: $OS_TYPE"
    exit 3
fi