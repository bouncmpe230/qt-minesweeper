# Cell Button

This component explains:

- `src/cellbutton.h`
- `src/cellbutton.cpp`

`CellButton` is a small custom widget that represents one visible square on the Minesweeper board.

## Purpose

A normal `QPushButton` can detect clicks, but it does not know:

- which row it belongs to
- which column it belongs to
- that right click should mean flag
- that `Ctrl` + left click should also behave like a flag action

`CellButton` adds exactly that behavior and nothing more.

## Class design

The class is very small by design:

```cpp
class CellButton : public QPushButton
{
    Q_OBJECT

signals:
    void revealRequested(int row, int column);
    void flagRequested(int row, int column);
```

This design is useful because the button does not mutate the game model directly. It simply emits the correct signal with its board coordinates.

## Constructor

The constructor stores the row and column and disables default behaviors that are not helpful here:

```cpp
CellButton::CellButton(int row, int column, QWidget *parent)
    : QPushButton(parent), m_row(row), m_column(column)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setFocusPolicy(Qt::NoFocus);
}
```

### Why these settings help

- `Qt::NoContextMenu` prevents a right-click menu from appearing.
- `Qt::NoFocus` keeps the UI focused on mouse interaction instead of button focus styling.

## Mouse handling

The most important method is the override of `mousePressEvent()`:

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

The real implementation also treats `Ctrl` + left click as a flag action.

## Why this is a good Qt example

This file is a strong example of when subclassing a Qt widget is worth it:

- the standard widget is already close to what we want
- only one event needs custom behavior
- the custom logic is small and easy to isolate

That is much cleaner than trying to make `MainWindow` inspect raw mouse events for every board cell.
