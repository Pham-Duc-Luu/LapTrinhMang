#!/bin/bash

# Dừng script ngay nếu có lỗi
set -e

# Tên output
OUTPUT="server"

# Compile
echo "🔨 Building server..."
g++ -std=c++17 -I./include server.cpp handler/handler.cpp  -o "$OUTPUT"

# Nếu build thành công thì chạy
echo "🚀 Running server..."
./"$OUTPUT"
