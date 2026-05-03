# Building Minesweeper in Qt: Starting with the Basics

This tutorial is meant to accompany the code in this repository. It begins with the core ideas you need in Qt, then shows how they come together in a complete Minesweeper project.

## 1. What Qt gives you

Qt is a C++ framework for building desktop and cross-platform applications. In this project we use **Qt Widgets**, which is the traditional UI toolkit in Qt.

Three ideas matter most at the beginning:

- **Widgets** are visible UI elements such as buttons, labels, and windows.
- **Layouts** arrange widgets without hard-coding pixel positions.
- **Signals and slots** connect user actions to application logic.

If you understand those three ideas, you can already build a surprisingly large amount of UI in Qt.

## 2. The project structure

This project is split into small, clear parts:

- `src/main.cpp` creates the Qt application and shows the main window.
- `src/mainwindow.cpp` builds the interface and reacts to user input.
- `src/minesweepergame.cpp` contains the game rules and board state.
- `src/cellbutton.cpp` gives each board cell custom mouse behavior.
- `src/tutorialdialog.cpp` provides a beginner help dialog.

That separation is important. The UI decides **how to present** the game, while `MinesweeperGame` decides **how the game works**.

## 3. Creating a Qt Widgets app

The build is defined in `CMakeLists.txt`.

Key ideas:

- `find_package(Qt6 REQUIRED COMPONENTS Widgets)` loads the Widgets module.
- `qt_add_executable(...)` creates the application target.
- `target_link_libraries(... Qt6::Widgets)` links the Qt Widgets library.

This is the standard modern Qt 6 CMake style.

## 4. The main window

`MainWindow` inherits from `QMainWindow`, which gives you:

- a central area for your content
- a menu bar
- a status bar

Inside the window, the project builds the interface using layouts instead of fixed coordinates:

- `QVBoxLayout` stacks sections vertically
- `QGridLayout` arranges controls and the board in rows and columns
- `QScrollArea` keeps the expert board usable on smaller screens and in browsers

This is one of the first good Qt habits to learn: let layouts do the positioning work.

## 5. Signals and slots in practice

Qt uses signals and slots to react to events.

Examples from this project:

- Clicking `New Game` starts a fresh board.
- Clicking `Beginner Tutorial` opens the help dialog.
- Clicking a `CellButton` either reveals a cell or toggles a flag.
- A `QTimer` updates the game clock every second.

Conceptually, it looks like this:

```cpp
connect(button, &QPushButton::clicked, this, &MainWindow::startNewGameFromSelection);
```

That line means: when the button emits `clicked`, call the `startNewGameFromSelection` function.

## 6. Why `CellButton` exists

Regular `QPushButton` already handles clicks, but Minesweeper needs custom cell behavior:

- left click should reveal
- right click should flag
- `Ctrl` + click should also flag for web-friendly play

So the project subclasses `QPushButton` and overrides `mousePressEvent`. That is a common Qt pattern when the default widget behavior is close to what you need, but not quite enough.

## 7. Keeping game logic separate

`MinesweeperGame` is a plain C++ class with no UI code inside it. It stores:

- board dimensions
- mine positions
- revealed cells
- flagged cells
- current game state

This separation makes the project easier to:

- test mentally
- refactor safely
- reuse with a different interface later

If the UI and game rules are tightly mixed together, small changes become harder than they need to be.

## 8. First-click-safe generation

One nice beginner-friendly feature in this project is first-click-safe generation.

The board is not fully finalized when the game starts. Instead:

1. The player clicks a cell.
2. The game places mines afterward.
3. The clicked cell, and usually its surrounding area, is kept safe.

This avoids frustrating instant losses and gives the player a usable opening position.

## 9. Flood fill for empty regions

When the player reveals a cell with zero neighboring mines, the game should open the nearby empty area automatically.

This project does that with a queue-based search:

- reveal the clicked zero cell
- visit its neighbors
- reveal neighboring zero cells too
- continue until the empty region is exhausted

That is a great example of how a simple algorithm can noticeably improve the user experience.

## 10. Updating the UI from the model

After each move, `MainWindow::refreshBoard()` redraws the button states based on the current game model.

Each button checks whether its cell is:

- hidden
- flagged
- revealed as a number
- revealed as a mine
- incorrectly flagged after a loss

This approach is straightforward and beginner-friendly because the rendering logic is centralized in one place.

## 11. Adding tutorial support

The project includes `TutorialDialog` for players and this written guide for developers.

That is intentional: small learning aids make a student project feel much more complete. They also make the WebAssembly deployment easier to understand for someone who opens the page without context.

## 12. Building for WebAssembly

Qt can compile C++ applications to WebAssembly so they run in the browser.

The workflow in `.github/workflows/deploy-wasm.yml` uses:

- `jurplel/install-qt-action` to install Qt for WebAssembly
- `qt-cmake` to configure the build with the right toolchain
- GitHub Pages actions to publish the generated site

When the workflow finishes, GitHub Pages serves the generated `.html`, `.js`, and `.wasm` files as a playable web version of the app.

That also fits the opening idea from the Qt slides: one codebase, multiple platforms.

## 13. Good next steps

Once you are comfortable with the basics, good extensions include:

- a custom board size dialog
- a restart button with preserved difficulty
- keyboard navigation
- high score persistence
- chord-click behavior for number cells
- sound or animation polish

## 14. What to remember

If you only keep a few things from this tutorial, keep these:

- Use layouts instead of fixed coordinates.
- Keep game rules separate from interface code.
- Use signals and slots to connect user actions to behavior.
- Start simple, then add polish once the structure is clean.

That foundation is enough to build a solid Qt project.
