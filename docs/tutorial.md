# Building Minesweeper in Qt: Starting with the Basics

This tutorial accompanies the code in this repository. It starts with the Qt basics from the course PDFs, then ties those ideas to the actual implementation in this project with short code snippets.

## 0. How to use this tutorial

Use this document in two passes:

- Read the main sections here first to understand the big picture.
- Open the detailed component notes in [components/README.md](components/README.md) when you want to study one class or subsystem in depth.

The PDFs in this directory, `PS10 - QT Programming.pdf` and `PS10 - QT_ Tetrix.pdf`, emphasize a few recurring ideas:

- `QApplication` starts the GUI event loop.
- `QMainWindow` gives the application a standard shell.
- layouts organize widgets cleanly.
- signals and slots connect UI events to behavior.
- custom widgets are useful when default controls are almost, but not quite, enough.
- separating UI from logic makes the project easier to grow.

This Minesweeper project follows that structure closely.

## 1. The smallest Qt application

Every Qt Widgets application needs one `QApplication` object. In this project, `src/main.cpp` is intentionally small:

```cpp
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Minesweeper");
    app.setOrganizationName("CMPE230");

    MainWindow window;
    window.show();

    return app.exec();
}
```

There are three key actions here:

- Create the Qt application object.
- Construct and show the main window.
- Start the event loop with `app.exec()`.

If you are new to Qt, this is the first pattern to memorize.

More detail: [components/application-bootstrap.md](components/application-bootstrap.md)

## 2. The project structure

The code is split into focused pieces:

- `src/main.cpp` boots the program.
- `src/mainwindow.cpp` builds the interface and coordinates user interaction.
- `src/minesweepergame.cpp` stores the rules and board state.
- `src/cellbutton.cpp` customizes mouse handling for board cells.
- `src/tutorialdialog.cpp` provides the beginner help window.
- `.github/workflows/deploy-wasm.yml` builds and deploys the browser version.

That split mirrors the Tetrix PDF: one part owns the window and presentation, another part owns the game behavior.

## 3. Building the project with Qt 6 and CMake

The course slides introduce `qmake`, but this repository uses the modern Qt 6 CMake style:

```cmake
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(minesweeper
    src/main.cpp
    src/mainwindow.cpp
    src/minesweepergame.cpp
    src/cellbutton.cpp
    src/tutorialdialog.cpp
)

target_link_libraries(minesweeper PRIVATE Qt6::Widgets)
```

This tells CMake:

- the project depends on Qt Widgets
- the executable is called `minesweeper`
- these source files are part of the application
- the final target must link against `Qt6::Widgets`

More detail: [components/application-bootstrap.md](components/application-bootstrap.md)

## 4. Why `MainWindow` exists

The course material uses `QMainWindow` when the application needs a real window shell with menu bars and status information. That is exactly what happens here.

The class definition already tells you what the main window controls:

```cpp
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:
    MinesweeperGame m_game;
    QTimer m_timer;
    int m_elapsedSeconds = 0;
    QVector<DifficultyPreset> m_presets;
```

That short section reveals a lot:

- `MainWindow` owns the game model.
- It also owns a `QTimer`.
- It tracks elapsed time.
- It stores difficulty presets for beginner, intermediate, and expert boards.

More detail: [components/main-window.md](components/main-window.md)

## 5. Layouts instead of manual positioning

One of the healthiest Qt habits is to let layouts manage placement. In `buildUi()`, the project uses a vertical layout for the page, a grid layout for controls, and another grid layout for the board:

```cpp
auto *rootLayout = new QVBoxLayout(centralWidget);
auto *controlsLayout = new QGridLayout();

m_boardWidget = new QWidget(scrollArea);
m_boardLayout = new QGridLayout(m_boardWidget);
scrollArea->setWidget(m_boardWidget);
```

This gives the app a few benefits immediately:

- the window resizes cleanly
- controls stay aligned
- the large expert board can sit inside a `QScrollArea`
- the same UI structure works better in the WebAssembly build

## 6. Signals and slots in real code

Signals and slots become much easier to understand when you see them in actual application code. Here is one of the simplest connections in the project:

```cpp
connect(m_newGameButton, &QPushButton::clicked, this, &MainWindow::startNewGameFromSelection);
```

This means:

- `m_newGameButton` emits `clicked`
- `MainWindow::startNewGameFromSelection()` runs in response

The timer uses the same idea:

```cpp
connect(&m_timer, &QTimer::timeout, this, [this]() {
    ++m_elapsedSeconds;
    updateHeader();
});
```

This is a good example of the Qt slides' main lesson: user events and time-based events can be handled with the same signals-and-slots model.

## 7. Why `CellButton` is a custom widget

The course PDFs spend time on custom widgets, and Minesweeper needs one. A normal `QPushButton` knows how to be clicked, but it does not know the rules of a Minesweeper cell.

`CellButton` subclasses `QPushButton` and translates mouse input into game-specific signals:

```cpp
if (event->button() == Qt::RightButton) {
    emit flagRequested(m_row, m_column);
    event->accept();
    return;
}

if (event->button() == Qt::LeftButton) {
    emit revealRequested(m_row, m_column);
    event->accept();
    return;
}
```

This is a clean design because the button does not try to update game state itself. It only reports intent:

- reveal this cell
- flag this cell

More detail: [components/cell-button.md](components/cell-button.md)

## 8. Keeping the game logic separate

`MinesweeperGame` is the logic model. It does not know about Qt layouts, buttons, stylesheets, or dialogs. It only knows how the board works.

The core reveal flow looks like this:

```cpp
if (!m_minesPlaced) {
    placeMines(row, column);
    computeAdjacencies();
    m_minesPlaced = true;
    m_state = GameState::Running;
}

target.revealed = true;
if (target.hasMine) {
    m_state = GameState::Lost;
    revealAllMines();
    return true;
}
```

That structure gives you two important behaviors:

- first-click-safe mine placement
- consistent reveal rules that the UI can trust

More detail: [components/game-logic.md](components/game-logic.md)

## 9. First-click-safe generation

Many beginner Minesweeper implementations place mines immediately when the board is created. This project waits until the first reveal. The relevant method is `placeMines()`:

```cpp
for (int row = safeRow - 1; row <= safeRow + 1; ++row) {
    for (int column = safeColumn - 1; column <= safeColumn + 1; ++column) {
        if (isInside(row, column)) {
            forbiddenCells.insert(indexFor(row, column));
        }
    }
}
```

This marks the clicked cell and its neighborhood as temporarily forbidden during mine placement. That small design choice makes the first move much friendlier for beginners.

## 10. Flood fill for empty areas

When the user clicks a zero-valued cell, the game should open a larger region automatically. The project does this with a queue-based search:

```cpp
QQueue<int> frontier;
frontier.enqueue(startIndex);

while (!frontier.isEmpty()) {
    const int currentIndex = frontier.dequeue();
    // inspect neighbors and reveal zero regions
}
```

This is a great teaching example because it connects a UI feature to a concrete algorithm. The player experiences it as convenience, but the code underneath is a structured traversal.

## 11. How the UI reacts to the model

After each move, `MainWindow::refreshBoard()` redraws every button based on the current `MinesweeperGame::Cell` state:

```cpp
if (cell.revealed && cell.hasMine) {
    text = "*";
} else if (cell.revealed) {
    text = cell.adjacentMines == 0 ? "" : QString::number(cell.adjacentMines);
} else if (cell.flagged) {
    text = "F";
}
```

This is a simple and reliable pattern:

- the model stores the truth
- the window asks the model for state
- the window redraws the view

That separation is one of the biggest reasons the code stays understandable.

## 12. Tutorial support inside the app

The project also contains a help dialog for beginner players. It is implemented with familiar Qt widgets:

```cpp
auto *browser = new QTextBrowser(this);
browser->setReadOnly(true);
browser->setMarkdown(R"(
## 1. Start with a single click
...
)");
```

This is useful for two reasons:

- it helps first-time users play the game
- it shows how documentation can live inside the application, not only in external files

More detail: [components/tutorial-dialog.md](components/tutorial-dialog.md)

## 13. WebAssembly deployment

The GitHub workflow builds the project with `qt-cmake` and publishes the generated browser files:

```yaml
- name: Configure project
  run: qt-cmake -S . -B build-wasm -G Ninja -DCMAKE_BUILD_TYPE=Release

- name: Build project
  run: cmake --build build-wasm --parallel
```

Then it copies the generated `.html`, `.js`, and `.wasm` files into a deployable folder for GitHub Pages.

This fits one of Qt's most useful promises: one codebase can target both desktop and browser builds with relatively small changes.

More detail: [components/build-and-deploy.md](components/build-and-deploy.md)

## 14. Recommended reading order after this

If you want to study the repository in depth, this order works well:

1. [components/application-bootstrap.md](components/application-bootstrap.md)
2. [components/main-window.md](components/main-window.md)
3. [components/cell-button.md](components/cell-button.md)
4. [components/game-logic.md](components/game-logic.md)
5. [components/tutorial-dialog.md](components/tutorial-dialog.md)
6. [components/build-and-deploy.md](components/build-and-deploy.md)

## 15. What to remember

If you only keep a few things from this project, keep these:

- Use layouts instead of fixed coordinates.
- Keep game logic separate from interface code.
- Use signals and slots everywhere user actions or timed actions matter.
- Introduce custom widgets only when standard widgets are close, but not quite enough.
- Start with a clean structure, then add polish.

That foundation is enough to build a strong Qt project.
