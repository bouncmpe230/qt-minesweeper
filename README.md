# Minesweeper in Qt

This repository contains a beginner-friendly Minesweeper project built with Qt Widgets and CMake. It includes:

- A desktop game with beginner, intermediate, and expert boards
- A built-in tutorial dialog for first-time players
- A written Qt tutorial that starts from the basics
- A GitHub Actions workflow that builds the app for Qt WebAssembly and deploys it to GitHub Pages

## Course alignment

This version intentionally follows the ideas in `PS10 - QT Programming.pdf` and `PS10 - QT_ Tetrix.pdf` from this directory:

- `QMainWindow` provides the application shell
- `QGridLayout`, `QVBoxLayout`, and standard widgets build the interface
- Qt signals and slots drive interaction
- `QTimer` handles elapsed time
- `CellButton` is a custom widget, similar to the slide deck's custom button examples
- `MinesweeperGame` keeps game logic separate from the main window, echoing the Tetrix separation between window and board logic

The main modernization is that this repository uses **Qt 6 + CMake** instead of the older `qmake` flow shown in the introductory slides.

## Features

- First-click-safe board generation
- Reveal flood fill for empty areas
- Flagging with right click, `Ctrl` + click, or the `Flag mode` toggle
- Live mine counter and timer
- Beginner tutorial designed for deployed web builds as well as desktop play

## Build locally

Desktop build:

```bash
cmake -S . -B build
cmake --build build
./build/minesweeper
```

If your generator puts the executable elsewhere, open the `minesweeper` binary from the build directory.

## Build for WebAssembly locally

If your Qt installation includes the WebAssembly toolchain:

```bash
qt-cmake -S . -B build-wasm -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-wasm
```

The generated web files appear in `build-wasm/`.

## GitHub Pages deployment

The workflow at `.github/workflows/deploy-wasm.yml`:

1. Installs Qt for WebAssembly on Ubuntu
2. Builds the app with `qt-cmake`
3. Collects the generated `.html`, `.js`, `.wasm`, and Qt loader files
4. Publishes the result to GitHub Pages

After pushing the repository to GitHub, enable Pages with `GitHub Actions` as the source.

## Project layout

- `src/` contains the Qt application
- `docs/tutorial.md` walks through the project starting with Qt basics
- `.github/workflows/deploy-wasm.yml` deploys the WebAssembly build
