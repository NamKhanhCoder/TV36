#!/bin/bash
echo "[BUILD] Dang quet toan bo file .cpp trong project..."
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR"

# Quet de quy tat ca cac file .cpp trong thu muc hien tai va thu muc con
SRC=$(find . -name "*.cpp" -not -path '*/.*')

# Neu khong tim thay file nao thi bao loi
if [ -z "$SRC" ]; then
    echo "[ERROR] Khong tim thay file .cpp nao!"
    exit 1
fi

echo "[BUILD] Dang bien dich..."
g++ -g -fdiagnostics-color=always $SRC -I"$DIR" -o main

# Kiem tra ket qua build
if [ $? -eq 0 ]; then
    echo "[SUCCESS] Build thanh cong! Da tao ra file main"
else
    echo "[ERROR] Build that bai! Vui long doc thong bao loi mau do o tren."
    exit 1
fi
