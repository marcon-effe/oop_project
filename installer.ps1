param(
    [string]$QMAKE = "qmake"
)

# Controllo sistema operativo
if ($env:OS -ne 'Windows_NT') {
    Write-Error "OS non supportato: $env:OS"
    exit 3
}

function Cleanup {
    Write-Host "Pulizia directory build e file temporanei..."
    Remove-Item -Recurse -Force build, oop_project.exe, *.pro, .qmake.stash, Makefile -ErrorAction SilentlyContinue

    Write-Host "Rimozione cartelle artisti in view/icons/..."
    Get-ChildItem -Path view/icons -Directory | Remove-Item -Recurse -Force -ErrorAction SilentlyContinue

    Write-Host "Rimozione file di autosave..."
    Remove-Item -Force save/autosave.json -ErrorAction SilentlyContinue
}

function Check-Xmllint {
    Write-Host "Verifica xmllint..."
    if (-not (Get-Command xmllint -ErrorAction SilentlyContinue)) {
        Write-Error "Errore: xmllint non trovato. Installa libxml2."
        exit 1
    }

    # Test schema
    $xml = [System.IO.Path]::GetTempFileName()
    $xsd = [System.IO.Path]::GetTempFileName()

    '<\?xml version="1.0"?><root/>' | Out-File $xml -Encoding utf8
    @"
<?xml version="1.0"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:element name="root" type="xs:string"/>
</xs:schema>
"@ | Out-File $xsd -Encoding utf8

    & xmllint --noout --schema $xsd $xml 2>$null
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Errore: xmllint senza supporto --schema."
        Remove-Item $xml, $xsd
        exit 1
    }

    Remove-Item $xml, $xsd
}

function Build-Windows {
    Cleanup
    Check-Xmllint

    Write-Host "qmake..."
    & $QMAKE "..\oop_project.pro"

    Write-Host "Compilazione..."
    if (Get-Command nmake -ErrorAction SilentlyContinue) {
        nmake /noLogo
    } elseif (Get-Command mingw32-make -ErrorAction SilentlyContinue) {
        mingw32-make -j $env:NUMBER_OF_PROCESSORS
    } else {
        Write-Error "Nessun tool di build trovato (nmake o mingw32-make)."
        exit 2
    }

    Write-Host "Avvio applicazione..."
    .\oop_project.exe
}

# Esecuzione principale
Build-Windows
