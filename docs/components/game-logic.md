# Game Logic

This component explains:

- `src/minesweepergame.h`
- `src/minesweepergame.cpp`

`MinesweeperGame` is the logic model of the project. It is the most important file if you want to understand how the game itself works.

## Purpose

`MinesweeperGame` is responsible for:

- board dimensions
- mine placement
- flag placement
- reveal behavior
- win/loss state
- neighbor mine counts

It does not know anything about:

- buttons
- layouts
- colors
- dialogs
- menus

That separation is intentional.

## Data model

Each board cell is stored as a small struct:

```cpp
struct Cell {
    bool hasMine = false;
    bool revealed = false;
    bool flagged = false;
    int adjacentMines = 0;
};
```

The game stores all cells in a `QVector<Cell>`, which acts like a flat array for the whole board.

## Resetting the game

`reset()` prepares a fresh board:

```cpp
m_rows = std::max(1, rows);
m_columns = std::max(1, columns);
const int availableCells = m_rows * m_columns;
m_mines = std::clamp(mines, 1, std::max(1, availableCells - 1));

m_cells = QVector<Cell>(availableCells);
m_state = GameState::Ready;
m_minesPlaced = false;
```

Important details:

- board dimensions are clamped to safe values
- mine count is also clamped
- cells are reinitialized
- the game starts in `Ready` state
- mines are not placed yet

## Revealing a cell

`revealCell()` is the central gameplay method:

```cpp
if (!m_minesPlaced) {
    placeMines(row, column);
    computeAdjacencies();
    m_minesPlaced = true;
    m_state = GameState::Running;
}
```

This is where first-click-safe behavior begins. The board is finalized only after the player's first reveal.

After that, the method:

- reveals the target cell
- ends the game if it contains a mine
- expands zero regions if needed
- checks whether the player has won

## Mine placement

`placeMines()` avoids the clicked cell and, when possible, its surrounding neighbors:

```cpp
for (int row = safeRow - 1; row <= safeRow + 1; ++row) {
    for (int column = safeColumn - 1; column <= safeColumn + 1; ++column) {
        if (isInside(row, column)) {
            forbiddenCells.insert(indexFor(row, column));
        }
    }
}
```

Then it builds a candidate list and randomly chooses mine locations.

This design gives beginners a much fairer first move.

## Computing adjacent counts

After mine placement, every safe cell needs its neighbor count:

```cpp
for (int neighborRow = row - 1; neighborRow <= row + 1; ++neighborRow) {
    for (int neighborColumn = column - 1; neighborColumn <= column + 1; ++neighborColumn) {
        if (m_cells[indexFor(neighborRow, neighborColumn)].hasMine) {
            ++target.adjacentMines;
        }
    }
}
```

This is classic Minesweeper logic: count how many mines touch each cell.

## Expanding zero regions

If a revealed cell has zero neighboring mines, the board should open the surrounding empty area:

```cpp
QQueue<int> frontier;
frontier.enqueue(startIndex);

while (!frontier.isEmpty()) {
    const int currentIndex = frontier.dequeue();
    // reveal safe neighbors
}
```

This is a breadth-first style expansion. It is more efficient and more maintainable than hard-coding repeated recursive calls in the UI layer.

## Flags

Flagging is intentionally simple:

```cpp
Cell &target = m_cells[indexFor(row, column)];
if (target.revealed) {
    return false;
}

target.flagged = !target.flagged;
```

The UI decides when to call this, but the model decides whether the action is legal.

## Ending the game

When the player hits a mine:

- the state becomes `Lost`
- all mines are revealed

When the player reveals every safe cell:

- the state becomes `Won`
- remaining mines are flagged automatically

The win check is straightforward:

```cpp
for (const Cell &cell : m_cells) {
    if (!cell.hasMine && !cell.revealed) {
        return;
    }
}

m_state = GameState::Won;
```

## Why this class matters

This class is a strong example of why separating logic from presentation helps:

- the rules are easier to reason about
- the UI code stays simpler
- future changes remain localized

If you later wanted a console version, a mobile version, or a different Qt front-end, this class could stay mostly the same.
