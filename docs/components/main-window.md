# Main Window

This component explains:

- `src/mainwindow.h`
- `src/mainwindow.cpp`

`MainWindow` is the controller and presentation shell of the application.

## Purpose

`MainWindow` is responsible for:

- building the visible interface
- managing widgets such as buttons, labels, and layouts
- starting and stopping the timer
- forwarding user actions to `MinesweeperGame`
- redrawing the board after each move
- opening the beginner tutorial dialog

## Key fields

The header already shows the main responsibilities:

```cpp
MinesweeperGame m_game;
QTimer m_timer;
int m_elapsedSeconds = 0;
QVector<DifficultyPreset> m_presets;
QVector<CellButton *> m_cellButtons;
TutorialDialog *m_tutorialDialog = nullptr;
```

### Why these members exist

- `m_game` owns the board state and rules.
- `m_timer` updates the elapsed time once per second.
- `m_presets` stores predefined board sizes.
- `m_cellButtons` stores pointers to the visible board cells.
- `m_tutorialDialog` is created lazily when the user first opens help.

## Constructor flow

The constructor does three high-level things:

```cpp
buildUi();
buildMenu();

connect(&m_timer, &QTimer::timeout, this, [this]() {
    ++m_elapsedSeconds;
    updateHeader();
});

startNewGameFromSelection();
```

This makes the startup sequence easy to follow:

1. Build the interface.
2. Build the menu.
3. Connect timer behavior.
4. Start the initial board.

## Building the UI

`buildUi()` creates the application layout from standard Qt widgets.

The structure is roughly:

- a title and subtitle
- a styled panel
- a controls grid with difficulty selection and action buttons
- a hint label
- a scroll area containing the Minesweeper board

Core layout code:

```cpp
auto *rootLayout = new QVBoxLayout(centralWidget);
auto *controlsLayout = new QGridLayout();

m_boardWidget = new QWidget(scrollArea);
m_boardLayout = new QGridLayout(m_boardWidget);
scrollArea->setWidget(m_boardWidget);
```


## Signals and slots

Some connections are straightforward:

```cpp
connect(m_newGameButton, &QPushButton::clicked, this, &MainWindow::startNewGameFromSelection);
connect(m_difficultyCombo, &QComboBox::currentIndexChanged, this, [this](int) {
    startNewGameFromSelection();
});
```

The timer uses a lambda so the update logic stays small and local:

```cpp
connect(&m_timer, &QTimer::timeout, this, [this]() {
    ++m_elapsedSeconds;
    updateHeader();
});
```

## Rebuilding the board

Whenever a new game starts, the visible grid must match the new board size:

```cpp
while (QLayoutItem *item = m_boardLayout->takeAt(0)) {
    if (QWidget *widget = item->widget()) {
        delete widget;
    }
    delete item;
}
```

After clearing the old grid, `rebuildBoard()` creates a fresh `CellButton` for every board position and connects each button's signals to the window logic.

## Handling cell interactions

The reveal path is where UI and model meet:

```cpp
if (!m_game.revealCell(selectedRow, selectedColumn)) {
    return;
}

refreshBoard();
updateHeader();
updateStatusMessage();
```

Important idea:

- `MainWindow` does not implement reveal rules itself.
- It asks `MinesweeperGame` to perform the move.
- Then it redraws the interface from the updated model state.

## Rendering the board

`refreshBoard()` translates model state into visible button appearance:

```cpp
if (cell.revealed && cell.hasMine) {
    text = "*";
} else if (cell.revealed) {
    text = cell.adjacentMines == 0 ? "" : QString::number(cell.adjacentMines);
} else if (cell.flagged) {
    text = "F";
}
```

This function is effectively the view renderer for the board.

## Status and feedback

`updateHeader()` keeps the mine counter and timer label current.

`updateStatusMessage()` updates the status bar based on the game state.

The reveal handler also shows `QMessageBox` dialogs on win and loss, which aligns with the teaching style in the course slides.

## Why this class matters

If you want to understand how a medium-sized Qt Widgets app is organized, `MainWindow` is the best file to study first because it shows:

- layouts
- signals and slots
- timers
- menus
- widget creation
- interaction with a separate logic model
