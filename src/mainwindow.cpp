#include "mainwindow.h"

#include <QAction>
#include <QFont>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>

#include "tutorialdialog.h"

namespace {

QString numberColor(int count)
{
    switch (count) {
    case 1:
        return "#1d4f9a";
    case 2:
        return "#2d7a3d";
    case 3:
        return "#a53a31";
    case 4:
        return "#5a3e9a";
    case 5:
        return "#7d4d21";
    case 6:
        return "#18707a";
    case 7:
        return "#3d3d3d";
    case 8:
        return "#7a2c4d";
    default:
        return "#233126";
    }
}

} // namespace

MainWindow::MainWindow()
{
    m_presets = {
        {"Beginner", 9, 9, 10},
        {"Intermediate", 16, 16, 40},
        {"Expert", 16, 30, 99},
    };

    buildUi();
    buildMenu();

    connect(&m_timer, &QTimer::timeout, this, [this]() {
        ++m_elapsedSeconds;
        updateHeader();
    });
    m_timer.setInterval(1000);

    startNewGameFromSelection();
}

void MainWindow::buildUi()
{
    setWindowTitle("Minesweeper Academy");
    resize(980, 760);

    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setSpacing(16);

    auto *titleLabel = new QLabel("Minesweeper", this);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: 700; color: #213027;");
    rootLayout->addWidget(titleLabel);

    m_subtitleLabel = new QLabel(
        "A Qt Widgets practice project with beginner guidance and browser-friendly controls.",
        this);
    m_subtitleLabel->setWordWrap(true);
    m_subtitleLabel->setStyleSheet("color: #4d5d53; font-size: 14px;");
    rootLayout->addWidget(m_subtitleLabel);

    auto *panel = new QWidget(this);
    panel->setObjectName("panel");
    panel->setStyleSheet(
        "#panel { background: #fffaf0; border: 1px solid #d8cab1; border-radius: 16px; }");
    rootLayout->addWidget(panel, 1);

    auto *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(18, 18, 18, 18);
    panelLayout->setSpacing(14);

    auto *controlsLayout = new QGridLayout();
    controlsLayout->setHorizontalSpacing(12);
    controlsLayout->setVerticalSpacing(10);

    auto *difficultyLabel = new QLabel("Difficulty", this);
    m_difficultyCombo = new QComboBox(this);
    for (const DifficultyPreset &preset : m_presets) {
        m_difficultyCombo->addItem(difficultySummary(preset));
    }

    m_newGameButton = new QPushButton("New Game", this);
    m_newGameButton->setObjectName("controlButton");
    m_tutorialButton = new QPushButton("Beginner Tutorial", this);
    m_tutorialButton->setObjectName("secondaryButton");
    m_flagModeCheckBox = new QCheckBox("Flag mode", this);
    m_mineCounterLabel = new QLabel(this);
    m_timerLabel = new QLabel(this);

    controlsLayout->addWidget(difficultyLabel, 0, 0);
    controlsLayout->addWidget(m_difficultyCombo, 0, 1);
    controlsLayout->addWidget(m_newGameButton, 0, 2);
    controlsLayout->addWidget(m_tutorialButton, 0, 3);
    controlsLayout->addWidget(m_flagModeCheckBox, 1, 0, 1, 2);
    controlsLayout->addWidget(m_mineCounterLabel, 1, 2);
    controlsLayout->addWidget(m_timerLabel, 1, 3);
    controlsLayout->setColumnStretch(1, 1);

    panelLayout->addLayout(controlsLayout);

    auto *hintLabel = new QLabel(
        "Left click reveals a cell. Right click, Ctrl + click, or Flag mode places a flag.",
        this);
    hintLabel->setWordWrap(true);
    hintLabel->setStyleSheet("color: #54655c; font-size: 13px;");
    panelLayout->addWidget(hintLabel);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent;");
    panelLayout->addWidget(scrollArea, 1);

    m_boardWidget = new QWidget(scrollArea);
    m_boardLayout = new QGridLayout(m_boardWidget);
    m_boardLayout->setSpacing(4);
    m_boardLayout->setContentsMargins(8, 8, 8, 8);
    m_boardLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    scrollArea->setWidget(m_boardWidget);

    setStyleSheet(R"(
        QWidget {
            background: #f4efe3;
            color: #233126;
            font-size: 14px;
        }
        QPushButton#controlButton {
            background: #295d4f;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 8px 14px;
            font-weight: 600;
        }
        QPushButton#controlButton:hover {
            background: #224b40;
        }
        QPushButton#secondaryButton {
            background: #dfd0b0;
            color: #2b382f;
            border: none;
            border-radius: 8px;
            padding: 8px 14px;
            font-weight: 600;
        }
        QPushButton#secondaryButton:hover {
            background: #d2c09a;
        }
        QComboBox {
            background: white;
            border: 1px solid #c8baa0;
            border-radius: 8px;
            padding: 6px 10px;
        }
        QCheckBox {
            spacing: 8px;
        }
        QStatusBar {
            background: #efe7d8;
        }
    )");

    connect(m_newGameButton, &QPushButton::clicked, this, &MainWindow::startNewGameFromSelection);
    connect(m_difficultyCombo, &QComboBox::currentIndexChanged, this, [this](int) {
        startNewGameFromSelection();
    });
    connect(m_tutorialButton, &QPushButton::clicked, this, [this]() {
        if (m_tutorialDialog == nullptr) {
            m_tutorialDialog = new TutorialDialog(this);
            connect(m_tutorialDialog, &TutorialDialog::beginnerGameRequested, this, [this]() {
                if (m_difficultyCombo->currentIndex() != 0) {
                    m_difficultyCombo->setCurrentIndex(0);
                    return;
                }

                startNewGameFromSelection();
            });
        }

        m_tutorialDialog->show();
        m_tutorialDialog->raise();
        m_tutorialDialog->activateWindow();
    });
}

void MainWindow::buildMenu()
{
    QMenu *gameMenu = menuBar()->addMenu("&Game");
    QAction *newGameAction = gameMenu->addAction("New Game");
    newGameAction->setShortcut(QKeySequence::New);
    connect(newGameAction, &QAction::triggered, this, &MainWindow::startNewGameFromSelection);

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *tutorialAction = helpMenu->addAction("Beginner Tutorial");
    connect(tutorialAction, &QAction::triggered, m_tutorialButton, &QPushButton::click);
}

void MainWindow::startNewGameFromSelection()
{
    const DifficultyPreset &preset = selectedPreset();

    m_timer.stop();
    m_elapsedSeconds = 0;
    m_flagModeCheckBox->setChecked(false);

    m_game.reset(preset.rows, preset.columns, preset.mines);
    rebuildBoard();
    refreshBoard();
    updateHeader();
    updateStatusMessage();
}

void MainWindow::rebuildBoard()
{
    while (QLayoutItem *item = m_boardLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            delete widget;
        }
        delete item;
    }

    m_cellButtons.clear();
    m_cellButtons.reserve(m_game.rows() * m_game.columns());

    for (int row = 0; row < m_game.rows(); ++row) {
        for (int column = 0; column < m_game.columns(); ++column) {
            auto *button = new CellButton(row, column, m_boardWidget);
            button->setFixedSize(34, 34);
            button->setStyleSheet(
                "background: #88a177; border: 1px solid #56705b; border-radius: 8px; font-weight: 700;");

            connect(button, &CellButton::revealRequested, this, [this](int selectedRow, int selectedColumn) {
                if (m_flagModeCheckBox->isChecked()) {
                    if (m_game.toggleFlag(selectedRow, selectedColumn)) {
                        refreshBoard();
                        updateHeader();
                        updateStatusMessage();
                    }
                    return;
                }

                const MinesweeperGame::GameState previousState = m_game.state();
                if (!m_game.revealCell(selectedRow, selectedColumn)) {
                    return;
                }

                if (previousState == MinesweeperGame::GameState::Ready
                    && m_game.state() == MinesweeperGame::GameState::Running) {
                    m_timer.start();
                }

                if (m_game.state() == MinesweeperGame::GameState::Won
                    || m_game.state() == MinesweeperGame::GameState::Lost) {
                    m_timer.stop();
                }

                const MinesweeperGame::GameState currentState = m_game.state();
                refreshBoard();
                updateHeader();
                updateStatusMessage();

                if (previousState != currentState) {
                    if (currentState == MinesweeperGame::GameState::Won) {
                        QMessageBox::information(
                            this,
                            "Board Cleared",
                            QString("You revealed every safe cell in %1.").arg(formattedTime()));
                    } else if (currentState == MinesweeperGame::GameState::Lost) {
                        QMessageBox::warning(
                            this,
                            "Mine Triggered",
                            "That reveal hit a mine. Try a new board and read the number clues one step at a time.");
                    }
                }
            });

            connect(button, &CellButton::flagRequested, this, [this](int selectedRow, int selectedColumn) {
                if (!m_game.toggleFlag(selectedRow, selectedColumn)) {
                    return;
                }

                refreshBoard();
                updateHeader();
                updateStatusMessage();
            });

            m_boardLayout->addWidget(button, row, column);
            m_cellButtons.append(button);
        }
    }
}

void MainWindow::refreshBoard()
{
    for (int row = 0; row < m_game.rows(); ++row) {
        for (int column = 0; column < m_game.columns(); ++column) {
            CellButton *button = m_cellButtons.at((row * m_game.columns()) + column);
            const MinesweeperGame::Cell &cell = m_game.cell(row, column);

            QString text;
            QString background = "#88a177";
            QString border = "#56705b";
            QString color = "#223126";

            if (cell.revealed && cell.hasMine) {
                text = "*";
                background = m_game.hasTriggeredMine(row, column) ? "#c44d3b" : "#e5c4ad";
                border = m_game.hasTriggeredMine(row, column) ? "#7d2a1f" : "#ac8c77";
                color = m_game.hasTriggeredMine(row, column) ? "#ffffff" : "#69251b";
            } else if (cell.revealed) {
                text = cell.adjacentMines == 0 ? "" : QString::number(cell.adjacentMines);
                background = "#f8f2e8";
                border = "#c6baa0";
                color = numberColor(cell.adjacentMines);
            } else if (cell.flagged && m_game.state() == MinesweeperGame::GameState::Lost && !cell.hasMine) {
                text = "X";
                background = "#f0d7d2";
                border = "#c39990";
                color = "#8a3026";
            } else if (cell.flagged) {
                text = "F";
                background = "#efbe57";
                border = "#9f7521";
                color = "#4f3310";
            }

            button->setText(text);
            button->setEnabled(!cell.revealed
                               && m_game.state() != MinesweeperGame::GameState::Won
                               && m_game.state() != MinesweeperGame::GameState::Lost);
            button->setStyleSheet(QString(
                "background: %1; border: 1px solid %2; border-radius: 8px; color: %3; font-weight: 700;")
                                      .arg(background, border, color));
        }
    }
}

void MainWindow::updateHeader()
{
    m_mineCounterLabel->setText(QString("Mines left: %1").arg(m_game.minesRemaining()));
    m_timerLabel->setText(QString("Time: %1").arg(formattedTime()));
}

void MainWindow::updateStatusMessage()
{
    switch (m_game.state()) {
    case MinesweeperGame::GameState::Ready:
        statusBar()->showMessage("Choose a cell to start. Your first reveal is always safe.");
        break;
    case MinesweeperGame::GameState::Running:
        statusBar()->showMessage("Clear every safe cell. Use Flag mode if you are playing in the browser.");
        break;
    case MinesweeperGame::GameState::Won:
        statusBar()->showMessage("Board cleared. Nice work.");
        break;
    case MinesweeperGame::GameState::Lost:
        statusBar()->showMessage("Mine triggered. Start a new game and try another pattern.");
        break;
    }
}

QString MainWindow::difficultySummary(const DifficultyPreset &preset) const
{
    return QString("%1 (%2 x %3, %4 mines)")
        .arg(preset.label)
        .arg(preset.rows)
        .arg(preset.columns)
        .arg(preset.mines);
}

QString MainWindow::formattedTime() const
{
    const int minutes = m_elapsedSeconds / 60;
    const int seconds = m_elapsedSeconds % 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

const MainWindow::DifficultyPreset &MainWindow::selectedPreset() const
{
    return m_presets.at(m_difficultyCombo->currentIndex());
}
