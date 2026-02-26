#!/bin/bash

CLEAR_TARGET="$(pwd)"/"$(dirname "$0")"
INSTALL_DIR="$HOME/cmake"

# =============================================================================|
rm -rfv "$INSTALL_DIR"
mkdir -p "$INSTALL_DIR"
cd "$HOME"
curl -LO https://github.com/Kitware/CMake/releases/download/v4.2.3/cmake-4.2.3-macos-universal.tar.gz
tar -xzf cmake-4.2.3-macos-universal.tar.gz -C "$INSTALL_DIR" --strip-components=1
rm -rfv cmake-4.2.3-macos-universal.tar.gz
# =============================================================================|

export PATH=$HOME/cmake/CMake.app/Contents/bin:$PATH

# =============================================================================|
git clone https://github.com/libsdl-org/SDL.git ~/SDLtmp
mkdir -p ~/SDLtmp/build
cd ~/SDLtmp/build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/SDL
make -j$(( $(sysctl -n hw.ncpu) + 2 ))
make install
rm -rf ~/SDLtmp

# Если нужна статическая или динамическая библиотека, можно указать:

# cmake .. -DBUILD_SHARED_LIBS=ON   # динамическая
# cmake .. -DBUILD_SHARED_LIBS=OFF  # статическая
# =============================================================================|

echo 'export PATH=$HOME/cmake/CMake.app/Contents/bin:$PATH' >> "$HOME/.zshrc"
echo 'export PATH=$HOME/cmake/CMake.app/Contents/bin:$PATH' >> "$HOME/.bashrc"

echo "======================================================"
echo "will be cleared: $CLEAR_TARGET"
echo "======================================================"
# rm -rf $CLEAR_TARGET
