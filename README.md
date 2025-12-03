# DuctedFanSim – Tooling & Workflow

This project is developed on **two platforms**:

- **Windows** – Visual Studio (full IDE, used for the final GUI + Windows build)
- **macOS** – Visual Studio Code (VS Code) + `clang++` (used for the core simulation code)

The C++ sources in `src/` and `include/` are intended to be **cross-platform**. All GUI / Windows-specific code should live in Windows-only projects (e.g., inside the Visual Studio solution).

---

## Editors / IDEs we use

### Visual Studio (Windows)

- Full integrated development environment (IDE) from Microsoft.
- Provides:
  - Built-in C++ project system (`.sln`, `.vcxproj`)
  - MSVC compiler, debugger, GUI tooling, and Windows SDK integration
  - Tight Git integration via the **Git** menu and **Git Changes** window (pull, push, sync). :contentReference[oaicite:0]{index=0}  
- This is where we will build the **final Windows GUI app** and the Windows `.exe`.

Windows-specific project files in this repo:

- `DuctedFanSim.sln`
- `DuctedFanSim.vcxproj`
- Any `Debug/` or `x64/` build artifacts (ignored by `.gitignore`)

### Visual Studio Code (VS Code)

- Lightweight **source code editor**, not a full IDE. :contentReference[oaicite:1]{index=1}  
- Cross-platform: runs the same on Windows, macOS, and Linux. :contentReference[oaicite:2]{index=2}  
- C/C++ support is provided by the **C/C++ extension**, which adds IntelliSense, debugging, and error checking. :contentReference[oaicite:3]{index=3}  
- On macOS, VS Code + `clang++` is used to:
  - Edit and run the **core simulation** (`src/` + `include/`),
  - Generate outputs like `output/flowfield.csv`.

VS Code configuration lives in:

- `.vscode/c_cpp_properties.json` – compiler path & include paths  
- `.vscode/tasks.json` – how to build the console simulation executable  
- `.vscode/launch.json` – how to run/debug the simulation from VS Code  

These files are mainly tuned for **macOS + clang++**, but can be adapted on Windows if someone wants to use VS Code there.

---

## Platform responsibilities

- **macOS (VS Code)**
  - Works on the **platform-neutral simulation core**:
    - Physics models, BEMT, duct models, IO, data structures
  - Uses `./ducted_fan_sim` (console app) built via VS Code tasks.
  - Ignores Windows-only files like `.sln`, `.vcxproj`, `*.exe`.

- **Windows (Visual Studio)**
  - Builds and runs the simulation and, later, the **GUI application**.
  - Uses `DuctedFanSim.sln` and Visual Studio’s C++ toolchain.
  - Can add extra projects (GUI, tools) to the solution that call into the shared simulation core.

All contributors share the same **C++ sources**. Only the build tooling differs per platform.

---

## Git & GitHub workflow (for everyone)

We share code via a GitHub repo:

- **Remote repo URL:** `https://github.com/ianwang02/DuctedFanSim.git`

Basic workflow (common to both VS Code and Visual Studio):

1. **Pull before work**
   - In VS Code: use **Source Control → “Sync Changes”** or `Git: Pull`.  
   - In Visual Studio: use **Git → Pull** or **Git → Fetch/Pull**. :contentReference[oaicite:4]{index=4}  
   - This ensures you start from the latest version of `main`.

2. **Edit code**
   - Modify `.cpp` / `.h` in `src/` and `include/`.
   - Keep core code portable (no Windows-only APIs in shared folders).

3. **Commit locally**
   - VS Code: stage files in the **Source Control** view → write a message → **Commit**.
   - Visual Studio: use the **Git Changes** window to stage, enter a commit message, and commit. :contentReference[oaicite:5]{index=5}  

4. **Push to GitHub**
   - VS Code: click **“Sync Changes”** (pushes after pulling).  
   - Visual Studio: **Git → Push** or push from the **Git Changes** window. :contentReference[oaicite:6]{index=6}  

5. **Repeat** for each logical change (feature, bug fix, etc.).

### Collaboration conventions

- **Core code stays portable** – anything in `src/` and `include/` should compile on both macOS and Windows with a standard C++17 compiler.
- **Windows-specific GUI code** should live in **Windows-only projects** in the Visual Studio solution.
- **Never commit build artifacts** (`Debug/`, `x64/`, `*.exe`, `*.obj`, etc.); `.gitignore` already filters most of these.
- Use **clear commit messages** that describe what changed (`"Add blade element interpolation"`, `"Refactor momentum disk model"`, etc.).

This setup lets:

- macOS users focus on the physics and simulation internals in VS Code, and  
- Windows users build the final GUI app in Visual Studio,

while everyone stays in sync through GitHub.
