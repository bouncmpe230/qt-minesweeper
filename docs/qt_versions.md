# Qt 4 / Qt 5 vs Qt 6+: High-level difference

|              | Old Qt (Qt 4 / 5)        | New Qt (Qt 6+)                                 |
| ------------ | ------------------------ | ---------------------------------------------- |
| Build system | qmake (`.pro`)           | **CMake (standard)**                           |
| Graphics     | OpenGL-heavy, mixed APIs | **Unified Rendering Hardware Interface (RHI)** |
| UI approach  | Widgets dominant         | **QML / Qt Quick first-class**                 |
| C++ standard | Older (C++11-ish)        | **Modern C++17+**                              |
| Performance  | Good                     | **Better, especially GPU & embedded**          |
| Ecosystem    | Qt-specific              | **More aligned with industry tools**           |

---

# Build system shift (biggest practical change)

### Old Qt

```pro
QT += widgets
SOURCES += main.cpp
```

### New Qt

```cmake
find_package(Qt6 REQUIRED COMPONENTS Widgets)
target_link_libraries(app Qt6::Widgets)
```

Qt **officially recommends CMake now**
qmake is essentially in maintenance mode.


# UI philosophy

### Old Qt

* **Qt Widgets = main approach**
* QML existed but was secondary

### New Qt

* **QML / Qt Quick is primary for UI**
* Widgets still supported but not evolving much

Think:

* Widgets → stable, classic desktop
* QML → modern, animated, responsive

---

# Rendering & graphics (major internal change)

### Old Qt

* Direct OpenGL usage
* Platform inconsistencies

### New Qt

* **RHI (Rendering Hardware Interface)**

👉 Means:

* Works across **OpenGL, Vulkan, Metal, Direct3D**
* Better portability (especially macOS/iOS/embedded)


# Modern C++ improvements

### Old Qt

* Custom Qt types everywhere (`QString`, `QList`, etc.)
* Limited STL usage

### New Qt

* Better integration with **STL and modern C++**
* Cleaner APIs, improved memory handling



# Performance & platform focus

### Old Qt

* Strong on desktop

### New Qt

* Optimized for:

  * **Embedded systems**
  * **Mobile**
  * **High-performance graphics**

---

#  Tooling evolution

| Tool             | Old Qt     | New Qt                |
| ---------------- | ---------- | --------------------- |
| Qt Creator       | Same IDE   | Still primary         |
| Qt Designer      | Widgets UI | Still used            |
| Qt Design Studio | Limited    | **Important for QML** |

