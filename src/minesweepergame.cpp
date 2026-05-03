#include "minesweepergame.h"

#include <algorithm>

#include <QQueue>
#include <QRandomGenerator>
#include <QSet>

void MinesweeperGame::reset(int rows, int columns, int mines)
{
    m_rows = std::max(1, rows);
    m_columns = std::max(1, columns);

    const int availableCells = m_rows * m_columns;
    m_mines = std::clamp(mines, 1, std::max(1, availableCells - 1));

    m_cells = QVector<Cell>(availableCells);
    m_state = GameState::Ready;
    m_minesPlaced = false;
    m_triggeredMineIndex.reset();
}

bool MinesweeperGame::revealCell(int row, int column)
{
    if (!isInside(row, column) || m_state == GameState::Won || m_state == GameState::Lost) {
        return false;
    }

    Cell &target = m_cells[indexFor(row, column)];
    if (target.revealed || target.flagged) {
        return false;
    }

    if (!m_minesPlaced) {
        placeMines(row, column);
        computeAdjacencies();
        m_minesPlaced = true;
        m_state = GameState::Running;
    }

    target.revealed = true;
    if (target.hasMine) {
        m_triggeredMineIndex = indexFor(row, column);
        m_state = GameState::Lost;
        revealAllMines();
        return true;
    }

    if (target.adjacentMines == 0) {
        revealZeroArea(indexFor(row, column));
    }

    checkForWin();
    return true;
}

bool MinesweeperGame::toggleFlag(int row, int column)
{
    if (!isInside(row, column) || m_state == GameState::Won || m_state == GameState::Lost) {
        return false;
    }

    Cell &target = m_cells[indexFor(row, column)];
    if (target.revealed) {
        return false;
    }

    target.flagged = !target.flagged;
    return true;
}

int MinesweeperGame::rows() const
{
    return m_rows;
}

int MinesweeperGame::columns() const
{
    return m_columns;
}

int MinesweeperGame::mineCount() const
{
    return m_mines;
}

int MinesweeperGame::flagsPlaced() const
{
    int totalFlags = 0;
    for (const Cell &cell : m_cells) {
        totalFlags += cell.flagged ? 1 : 0;
    }
    return totalFlags;
}

int MinesweeperGame::minesRemaining() const
{
    return m_mines - flagsPlaced();
}

MinesweeperGame::GameState MinesweeperGame::state() const
{
    return m_state;
}

bool MinesweeperGame::firstMovePending() const
{
    return !m_minesPlaced;
}

bool MinesweeperGame::hasTriggeredMine(int row, int column) const
{
    return m_triggeredMineIndex.has_value() && m_triggeredMineIndex.value() == indexFor(row, column);
}

const MinesweeperGame::Cell &MinesweeperGame::cell(int row, int column) const
{
    return m_cells[indexFor(row, column)];
}

bool MinesweeperGame::isInside(int row, int column) const
{
    return row >= 0 && row < m_rows && column >= 0 && column < m_columns;
}

int MinesweeperGame::indexFor(int row, int column) const
{
    return (row * m_columns) + column;
}

void MinesweeperGame::placeMines(int safeRow, int safeColumn)
{
    QSet<int> forbiddenCells;
    for (int row = safeRow - 1; row <= safeRow + 1; ++row) {
        for (int column = safeColumn - 1; column <= safeColumn + 1; ++column) {
            if (isInside(row, column)) {
                forbiddenCells.insert(indexFor(row, column));
            }
        }
    }

    QVector<int> candidates;
    candidates.reserve(m_cells.size());
    for (int index = 0; index < m_cells.size(); ++index) {
        if (!forbiddenCells.contains(index)) {
            candidates.append(index);
        }
    }

    if (candidates.size() < m_mines) {
        forbiddenCells.clear();
        forbiddenCells.insert(indexFor(safeRow, safeColumn));
        candidates.clear();
        for (int index = 0; index < m_cells.size(); ++index) {
            if (!forbiddenCells.contains(index)) {
                candidates.append(index);
            }
        }
    }

    for (int mineIndex = 0; mineIndex < m_mines; ++mineIndex) {
        const int randomCandidate = QRandomGenerator::global()->bounded(candidates.size());
        const int cellIndex = candidates.takeAt(randomCandidate);
        m_cells[cellIndex].hasMine = true;
    }
}

void MinesweeperGame::computeAdjacencies()
{
    for (int row = 0; row < m_rows; ++row) {
        for (int column = 0; column < m_columns; ++column) {
            Cell &target = m_cells[indexFor(row, column)];
            target.adjacentMines = 0;

            if (target.hasMine) {
                continue;
            }

            for (int neighborRow = row - 1; neighborRow <= row + 1; ++neighborRow) {
                for (int neighborColumn = column - 1; neighborColumn <= column + 1; ++neighborColumn) {
                    if ((neighborRow == row && neighborColumn == column) || !isInside(neighborRow, neighborColumn)) {
                        continue;
                    }

                    if (m_cells[indexFor(neighborRow, neighborColumn)].hasMine) {
                        ++target.adjacentMines;
                    }
                }
            }
        }
    }
}

void MinesweeperGame::revealZeroArea(int startIndex)
{
    QQueue<int> frontier;
    frontier.enqueue(startIndex);

    while (!frontier.isEmpty()) {
        const int currentIndex = frontier.dequeue();
        const int row = currentIndex / m_columns;
        const int column = currentIndex % m_columns;

        for (int neighborRow = row - 1; neighborRow <= row + 1; ++neighborRow) {
            for (int neighborColumn = column - 1; neighborColumn <= column + 1; ++neighborColumn) {
                if ((neighborRow == row && neighborColumn == column) || !isInside(neighborRow, neighborColumn)) {
                    continue;
                }

                Cell &neighbor = m_cells[indexFor(neighborRow, neighborColumn)];
                if (neighbor.revealed || neighbor.flagged || neighbor.hasMine) {
                    continue;
                }

                neighbor.revealed = true;
                if (neighbor.adjacentMines == 0) {
                    frontier.enqueue(indexFor(neighborRow, neighborColumn));
                }
            }
        }
    }
}

void MinesweeperGame::revealAllMines()
{
    for (Cell &cell : m_cells) {
        if (cell.hasMine) {
            cell.revealed = true;
        }
    }
}

void MinesweeperGame::flagRemainingMines()
{
    for (Cell &cell : m_cells) {
        if (cell.hasMine) {
            cell.flagged = true;
        }
    }
}

void MinesweeperGame::checkForWin()
{
    for (const Cell &cell : m_cells) {
        if (!cell.hasMine && !cell.revealed) {
            return;
        }
    }

    m_state = GameState::Won;
    flagRemainingMines();
}

