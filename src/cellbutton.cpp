#include "cellbutton.h"

#include <QMouseEvent>

CellButton::CellButton(int row, int column, QWidget *parent)
    : QPushButton(parent), m_row(row), m_column(column)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setFocusPolicy(Qt::NoFocus);
}

void CellButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        emit flagRequested(m_row, m_column);
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton && event->modifiers().testFlag(Qt::ControlModifier)) {
        emit flagRequested(m_row, m_column);
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        emit revealRequested(m_row, m_column);
        event->accept();
        return;
    }

    QPushButton::mousePressEvent(event);
}

