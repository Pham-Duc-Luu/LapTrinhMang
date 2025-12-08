#!/bin/bash

# Dừng script ngay nếu có lỗi
set -e

# Tên output
OUTPUT="client"


# Compile
echo "🔨 Building client..."

g++ client.cpp -o "$OUTPUT"


# Nếu build thành công thì chạy
echo "🚀 Running server..."
./"$OUTPUT" 127.0.0.1 8080