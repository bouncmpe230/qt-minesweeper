#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>
#include <QTimer>
#include <QVector>

#include "cellbutton.h"
#include "minesweepergame.h"

class TutorialDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:
    struct DifficultyPreset {
        QString label;
        int rows = 0;
        int columns = 0;
        int mines = 0;
    };

    void buildUi();
    void buildMenu();
    void startNewGameFromSelection();
    void rebuildBoard();
    void refreshBoard();
    void updateHeader();
    void updateStatusMessage();
    QString difficultySummary(const DifficultyPreset &preset) const;
    QString formattedTime() const;
    const DifficultyPreset &selectedPreset() const;

    MinesweeperGame m_game;
    QTimer m_timer;
    int m_elapsedSeconds = 0;
    QVector<DifficultyPreset> m_presets;

    QComboBox *m_difficultyCombo = nullptr;
    QPushButton *m_newGameButton = nullptr;
    QPushButton *m_tutorialButton = nullptr;
    QCheckBox *m_flagModeCheckBox = nullptr;
    QLabel *m_mineCounterLabel = nullptr;
    QLabel *m_timerLabel = nullptr;
    QLabel *m_subtitleLabel = nullptr;
    QWidget *m_boardWidget = nullptr;
    QGridLayout *m_boardLayout = nullptr;
    QVector<CellButton *> m_cellButtons;

    TutorialDialog *m_tutorialDialog = nullptr;
};

