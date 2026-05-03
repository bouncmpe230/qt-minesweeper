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
2. Configure GitHub Pages.
3. Install Qt for WebAssembly.
4. Fix execute permission on the Qt wrapper scripts.
5. Install the matching Emscripten SDK version.
6. Source `emsdk_env.sh` and configure the project with `qt-cmake`.
7. Source `emsdk_env.sh` again and build the project.
8. Collect the generated browser files.
9. Upload and deploy them to GitHub Pages.

## Important workflow snippets

Qt installation:

```yaml
- name: Install Qt for WebAssembly
  uses: jurplel/install-qt-action@v4
  with:
    version: "6.9.2"
    host: "all_os"
    target: "wasm"
    arch: "wasm_singlethread"
```

For Qt `6.9.x`, the workflow pairs the build with Emscripten `3.1.70`.

Build configuration:

```yaml
- name: Fix Qt WebAssembly tool permissions
  run: chmod +x "$QT_ROOT_DIR/bin/"*

- name: Install Emscripten SDK
  run: |
    git clone https://github.com/emscripten-core/emsdk.git "$RUNNER_TEMP/emsdk"
    cd "$RUNNER_TEMP/emsdk"
    ./emsdk install 3.1.70
    ./emsdk activate 3.1.70
    echo "EMSDK=$RUNNER_TEMP/emsdk" >> "$GITHUB_ENV"

- name: Configure CMake
  run: |
    source "$EMSDK/emsdk_env.sh"
    "$QT_ROOT_DIR/bin/qt-cmake" -G Ninja -S . -B build-wasm -DCMAKE_BUILD_TYPE=Release
```

Deploy artifact preparation:

```yaml
- name: Stage Pages artifact
  run: |
    mkdir -p site
    cp build-wasm/*.html site/
    cp build-wasm/*.js site/
    cp build-wasm/*.wasm site/
```

## Why this component matters

The WebAssembly workflow turns the project from a local classroom exercise into something you can share and demo through the browser.

It also reinforces one of Qt's central promises from the course material:

- one codebase
- multiple targets
