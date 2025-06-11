#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

# Rileva sistema operativo e comando qmake
OS_TYPE="$(uname)"
QMAKE="${1:-qmake}"

# Pulisce build, file temporanei e cartelle icone artisti
cleanup() {
    echo "Pulizia directory build e file temporanei..."
    rm -rf build/ oop_project oop_project.pro .qmake.stash Makefile
    echo "Rimozione cartelle artisti in view/icons/..."
    rm -rf view/icons/*/
    echo "Rimozione file di autosave..."
    rm -f saves/autosave.json
}

# Verifica xmllint e supporto schema
check_xmllint() {
    if ! command -v xmllint &>/dev/null; then
        echo "Errore: xmllint non trovato. Installa libxml2." >&2
        exit 1
    fi

  # Test schema
    local xml=$(mktemp tmpXXXX.xml)
    local xsd=$(mktemp tmpXXXX.xsd)
    echo '<?xml version="1.0"?><root/>' > "$xml"
    cat > "$xsd" <<EOF
<?xml version="1.0"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
<xs:element name="root" type="xs:string"/>
</xs:schema>
EOF

  if ! xmllint --noout --schema "$xsd" "$xml" &>/dev/null; then
    echo "Errore: xmllint senza supporto --schema." >&2
    rm -f "$xml" "$xsd"
    exit 1
  fi

    rm -f "$xml" "$xsd"
}

# Compilazione su Linux
build_linux() {
    echo "Pulizia..."
    cleanup
    mkdir -p build && cd build

    echo "qmake..."
    "$QMAKE" ../oop_project.pro

    echo "Compilazione..."
    make -j"$(nproc)"

    echo "Sposto eseguibile..."
    mv oop_project ../
    cd ..

    echo "Avvio applicazione..."
    ./oop_project
}

# Compilazione su macOS
build_macos() {
    echo "Pulizia..."
    cleanup

    echo "qmake -project..."
    "$QMAKE" -project -o oop_project.pro

    echo "Aggiungo moduli Qt e configurazioni..."
    printf "\nQT += core gui widgets xml\nCONFIG -= app_bundle\nRESOURCES += view/resources.qrc\nDESTDIR = .\nOBJECTS_DIR = build\nMOC_DIR = build\nRCC_DIR = build\nUI_DIR = build\n" >> oop_project.pro

    mkdir -p build

    echo "qmake..."
    "$QMAKE" oop_project.pro

    echo "Compilazione..."
    make -j"$(sysctl -n hw.ncpu)"

    echo "Avvio applicazione..."
    ./oop_project
}

# Esecuzione principale
check_xmllint
case "$OS_TYPE" in
    Linux)   build_linux ;;  
    Darwin)  build_macos ;;  
    *)       echo "OS non supportato: $OS_TYPE" >&2; exit 3 ;;  
 esac
