# Splanner2 — Qt Study Planner

A desktop study planner built with C++ and Qt, designed to help students organize tasks, track deadlines, and stay on top of their study schedule through a clean, simple GUI.

## Features

- **Task Management** — Add, edit, and remove study tasks with core logic handled independently of the UI
- **Qt-based Graphical Interface** — Native desktop GUI built with Qt widgets for a responsive, cross-platform experience
- **Calendar View** — Visualize tasks and deadlines at a glance *(in progress)*
- *(Add/remove bullets here as your Day 3 work finalizes — e.g. task prioritization, due-date sorting, completion tracking)*

## Tech Stack

- **Language:** C++
- **Framework:** Qt (Widgets)
- **Build System:** qmake (`.pro` file)

## Project Structure

```
Splanner2/
├── StudyPlanner.pro     # Qt project/build configuration
├── Task.h                # Task data model
├── TaskManager.h/.cpp     # Core task management logic
├── main_qt.cpp            # Application entry point
├── mainwindow.h/.cpp       # Main GUI window
```

## Getting Started

### Prerequisites
- [Qt](https://www.qt.io/download) (Qt Creator recommended) with a compatible C++ compiler (MSVC or MinGW on Windows)

### Build & Run
1. Clone the repository:
   ```
   git clone https://github.com/<your-username>/Splanner2.git
   ```
2. Open `StudyPlanner.pro` in Qt Creator
3. Build and run the project (Ctrl+R in Qt Creator)

## Project Status

🚧 **In active development** — core task logic and GUI are complete; calendar view and final polish are in progress.

## Team

Built collaboratively as a group project.
*(Add teammates' names/GitHub handles here)*

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.
