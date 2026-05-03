# Tutorial Dialog

This component explains:

- `src/tutorialdialog.h`
- `src/tutorialdialog.cpp`

`TutorialDialog` is the in-app help window for first-time players.

## Purpose

This dialog gives the application a built-in beginner guide so users do not need to leave the program to learn basic controls.

It is especially helpful for:

- new desktop users
- browser users in the WebAssembly build
- course demos where you want the help inside the app itself

## Class design

The class is intentionally minimal:

```cpp
class TutorialDialog : public QDialog
{
    Q_OBJECT

signals:
    void beginnerGameRequested();
};
```

The one custom signal lets the dialog ask the main window to start a beginner board.

## UI structure

The constructor builds the dialog with standard widgets:

```cpp
auto *layout = new QVBoxLayout(this);
auto *title = new QLabel("Minesweeper Basics", this);
auto *browser = new QTextBrowser(this);
auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
```

This is a good example of how far you can get in Qt with just:

- a layout
- a label
- a text display widget
- a button box

## Tutorial content

The help text is written with `setMarkdown()`:

```cpp
browser->setMarkdown(R"(
## 1. Start with a single click
...
## 6. Best first step
)");
```

Using Markdown keeps the help readable in source form and easy to expand later.

## Action button

The extra action button starts a beginner board:

```cpp
connect(beginnerButton, &QPushButton::clicked, this, [this]() {
    emit beginnerGameRequested();
    accept();
});
```

This is a nice pattern because:

- the dialog does not manipulate `MainWindow` directly
- the dialog emits intent
- the main window decides how to respond

That is a recurring theme across the whole repository.
