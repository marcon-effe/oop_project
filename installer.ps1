Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Cleanup {
    Write-Host "Pulizia directory build e file temporanei..."
    Remove-Item -Recurse -Force -ErrorAction SilentlyContinue build, oop_project.exe, oop_project.pro, .qmake.stash, Makefile
    Write-Host "Rimozione cartelle artisti in view/icons/..."
    Get-ChildItem -Directory -Path "view/icons" | Remove-Item -Recurse -Force -ErrorAction SilentlyContinue
}

function Check-Xmllint {
    if (-not (Get-Command "xmllint.exe" -ErrorAction SilentlyContinue)) {
        Write-Error "Errore: xmllint non trovato. Installa libxml2."
        exit 1
    }

    $xml = New-TemporaryFile
    $xsd = New-TemporaryFile

    '<?xml version="1.0"?><root/>' | Set-Content $xml
    @"
<?xml version='1.0'?>
<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>
  <xs:element name='root' type='xs:string'/>
</xs:schema>
"@ | Set-Content $xsd

    & xmllint --noout --schema $xsd $xml
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Errore: xmllint non supporta --schema."
        exit 1
    }

    Remove-Item $xml, $xsd
    Write-Host "[✓] xmllint supporta --schema.`n"
}

function Find-Make {
    $preferred = "C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe"
    if (Test-Path $preferred) {
        return $preferred
    }

    if (Get-Command "mingw32-make.exe" -ErrorAction SilentlyContinue) {
        return "mingw32-make.exe"
    }

    $fallback = Get-ChildItem "C:\Qt\Tools\" -Recurse -Filter mingw32-make.exe -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($fallback) {
        return $fallback.FullName
    }

    Write-Error "mingw32-make non trovato. Aggiungilo al PATH o installa MinGW 11.2.0."
    exit 1
}

function Generate-ProFile {
    Write-Host "Genero il file oop_project.pro..."
    Remove-Item -Force -ErrorAction SilentlyContinue oop_project.pro

    Add-Content oop_project.pro "QT += core gui widgets xml"
    Add-Content oop_project.pro "CONFIG -= app_bundle"
    Add-Content oop_project.pro "RESOURCES += view/resources.qrc"
    Add-Content oop_project.pro "DESTDIR = ."
    Add-Content oop_project.pro "OBJECTS_DIR = build"
    Add-Content oop_project.pro "MOC_DIR = build"
    Add-Content oop_project.pro "RCC_DIR = build"
    Add-Content oop_project.pro "UI_DIR = build"
    Add-Content oop_project.pro ""

    Add-Content oop_project.pro "SOURCES += \\"
    Get-ChildItem -Recurse -Include *.cpp | ForEach-Object {
        $relPath = $_.FullName.Replace((Get-Location).Path + "\", "").Replace("\", "/")
        Add-Content oop_project.pro "    $relPath \\"
    }

    Add-Content oop_project.pro ""
    Add-Content oop_project.pro "HEADERS += \\"
    Get-ChildItem -Recurse -Include *.h | ForEach-Object {
        $relPath = $_.FullName.Replace((Get-Location).Path + "\", "").Replace("\", "/")
        Add-Content oop_project.pro "    $relPath \\"
    }
}

function Build-Windows {
    Cleanup
    Check-Xmllint
    Generate-ProFile

    if (-not (Test-Path build)) {
        New-Item -ItemType Directory -Path build | Out-Null
    }

    Push-Location build
    Write-Host "qmake..."
    & qmake "..\oop_project.pro"

    $make = Find-Make
    Write-Host "Compilazione..."
    & "$make" -j $env:NUMBER_OF_PROCESSORS
    Pop-Location

    Write-Host "Sposto eseguibile..."
    Move-Item -Force "build/oop_project.exe" "oop_project.exe"

    Write-Host "Avvio applicazione..."
    .\oop_project.exe
}

Build-Windows
