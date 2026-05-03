# Build and Deploy

This component explains:

- local desktop builds
- local WebAssembly builds
- `.github/workflows/deploy-wasm.yml`

## Purpose

This part of the project turns the source code into:

- a desktop application
- a browser build deployed through GitHub Pages

## Local desktop build

The standard desktop build is:

```bash
cmake -S . -B build
cmake --build build
./build/minesweeper
```

This uses the normal Qt installation on your machine.

## Local WebAssembly build

If your Qt installation includes WebAssembly support, the local build is:

```bash
qt-cmake -S . -B build-wasm -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-wasm
```

Qt then generates a web-ready set of files such as:

- `.html`
- `.js`
- `.wasm`

## GitHub Actions workflow

The workflow file is `.github/workflows/deploy-wasm.yml`.

The overall job sequence is:

1. Check out the repository.
2. Install build tools.
3. Install Qt for WebAssembly.
4. Configure the project with `qt-cmake`.
5. Build the project.
6. Collect the generated browser files.
7. Upload and deploy them to GitHub Pages.

## Important workflow snippets

Qt installation:

```yaml
- name: Install Qt for WebAssembly
  uses: jurplel/install-qt-action@v4
  with:
    version: "6.9.2"
    host: "linux"
    target: "desktop"
    arch: "wasm_singlethread"
```

Build configuration:

```yaml
- name: Configure project
  run: qt-cmake -S . -B build-wasm -G Ninja -DCMAKE_BUILD_TYPE=Release
```

Deploy artifact preparation:

```yaml
- name: Collect web files
  run: |
    mkdir -p dist
    cp build-wasm/minesweeper.html dist/index.html
    cp build-wasm/minesweeper.js dist/
    cp build-wasm/minesweeper.wasm dist/
```

## Why this component matters

The WebAssembly workflow turns the project from a local classroom exercise into something you can share and demo through the browser.

It also reinforces one of Qt's central promises from the course material:

- one codebase
- multiple targets
