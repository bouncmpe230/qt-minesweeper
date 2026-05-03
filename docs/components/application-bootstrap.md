# Application Bootstrap

This component covers the files that make the application start and build correctly:

- `src/main.cpp`
- `CMakeLists.txt`

## Purpose

These files answer two questions:

- How does the program start?
- How does Qt know how to build it?

## `main.cpp`

`main.cpp` is intentionally tiny:

```cpp
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Minesweeper");
    app.setOrganizationName("CMPE230");

    MainWindow window;
    window.show();

    return app.exec();
}
```

### Why this matters

- `QApplication` initializes Qt's widget system.
- `MainWindow` is the main user-facing window.
- `window.show()` makes the window visible.
- `app.exec()` starts the event loop and keeps the app responsive.

Without `app.exec()`, the program would create the window and then immediately exit.

## `CMakeLists.txt`

The build configuration uses modern Qt 6 CMake commands:

```cmake
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(minesweeper
    src/main.cpp
    src/mainwindow.cpp
    src/minesweepergame.cpp
    src/cellbutton.cpp
    src/tutorialdialog.cpp
)

target_link_libraries(minesweeper PRIVATE Qt6::Widgets)
```

### What each line does

- `find_package(...)` locates the installed Qt Widgets module.
- `qt_add_executable(...)` defines the app target and its source files.
- `target_link_libraries(...)` links the executable against Qt Widgets.


## When to edit this component

You usually change these files when:

- adding new source files
- enabling more Qt modules
- changing application metadata
- adjusting install behavior
