#include "tutorialdialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

TutorialDialog::TutorialDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Beginner Tutorial");
    resize(520, 420);

    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel("Minesweeper Basics", this);
    title->setStyleSheet("font-size: 22px; font-weight: 700; color: #203027;");
    layout->addWidget(title);

    auto *intro = new QLabel(
        "This quick guide is written for first-time players and works well for the web build too.",
        this);
    intro->setWordWrap(true);
    layout->addWidget(intro);

    auto *browser = new QTextBrowser(this);
    browser->setReadOnly(true);
    browser->setOpenExternalLinks(false);
    browser->setMarkdown(R"(
## 1. Start with a single click

Choose any hidden cell to begin. This version keeps your first reveal safe so you can learn the board without losing immediately.

## 2. Read the numbers

When a cell shows a number, it tells you how many mines touch that square in the surrounding eight positions.

## 3. Mark likely mines

Use one of these controls to place a flag:

- Right click a hidden cell
- Hold `Ctrl` and left click
- Turn on `Flag mode` and then click cells normally

## 4. Clear empty areas

If you reveal a cell with no nearby mines, the game opens the surrounding empty area automatically.

## 5. Win condition

You win by revealing every safe cell. Flags help you reason about the board, but only safe reveals decide the result.

## 6. Best first step

Pick the **Beginner** board, make one reveal, and focus on reading small number patterns before moving to larger boards.
)");
    layout->addWidget(browser, 1);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    auto *beginnerButton = new QPushButton("Start Beginner Board", this);
    buttons->addButton(beginnerButton, QDialogButtonBox::ActionRole);

    connect(beginnerButton, &QPushButton::clicked, this, [this]() {
        emit beginnerGameRequested();
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(buttons);
}

