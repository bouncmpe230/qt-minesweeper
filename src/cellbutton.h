#pragma once

#include <QPushButton>

class CellButton : public QPushButton
{
    Q_OBJECT

public:
    CellButton(int row, int column, QWidget *parent = nullptr);

signals:
    void revealRequested(int row, int column);
    void flagRequested(int row, int column);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_row = 0;
    int m_column = 0;
};

