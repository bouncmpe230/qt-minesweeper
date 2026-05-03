#pragma once

#include <optional>

#include <QVector>

class MinesweeperGame
{
public:
    enum class GameState {
        Ready,
        Running,
        Won,
        Lost
    };

    struct Cell {
        bool hasMine = false;
        bool revealed = false;
        bool flagged = false;
        int adjacentMines = 0;
    };

    void reset(int rows, int columns, int mines);

    bool revealCell(int row, int column);
    bool toggleFlag(int row, int column);

    int rows() const;
    int columns() const;
    int mineCount() const;
    int flagsPlaced() const;
    int minesRemaining() const;

    GameState state() const;
    bool firstMovePending() const;
    bool hasTriggeredMine(int row, int column) const;

    const Cell &cell(int row, int column) const;

private:
    bool isInside(int row, int column) const;
    int indexFor(int row, int column) const;
    void placeMines(int safeRow, int safeColumn);
    void computeAdjacencies();
    void revealZeroArea(int startIndex);
    void revealAllMines();
    void flagRemainingMines();
    void checkForWin();

    int m_rows = 0;
    int m_columns = 0;
    int m_mines = 0;
    QVector<Cell> m_cells;
    GameState m_state = GameState::Ready;
    bool m_minesPlaced = false;
    std::optional<int> m_triggeredMineIndex;
};

