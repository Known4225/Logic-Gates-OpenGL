# Logic Gates

### Compile (windows 64 bit):
gcc logicgatesWindows.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -O3 -o logicgatesWindows.exe
Then run logicgatesWindows.exe

### Compile (linux):
gcc logicgatesLinux.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o logicgatesLinux.o
Then run ./logicgatesLinux.o

To load a file, type:
logicgates.exe {filename.txt}
in the terminal (with no brackets)
I've included some demo projects in the "Demos" folder
Files can also be loaded in via the GUI menu

This application is designed to be usable with just a mouse, but there are
keybind shortcuts to make building circuits faster.

## Keybinds:
click and drag - Place components, move components, or move screen\
click on a POWER component to toggle it on/off\
space + click + drag - create wire\
1, e, p - POWER component (input/output)\
2, n - NOT gate\
3, a - AND gate\
4, q, o - OR gate\
5 - XOR gate\
6 - NOR gate\
7 - NAND gate\
8 - BUFFER gate\
x - delete component\
shift + click + drag - select\
shift + click - add component to selection\
c - copy/paste selected\
scroll wheel (or up and down arrows) - zoom\
up and down arrows - zoom\
space + scroll wheel - rotate component (coarse)\
sideways arrows - rotate component (fine)\
space + sideways arrows - rotate selected (fine)\
k - export to file\
h - toggle sidebar\
w - toggle wireMode\
t - toggle theme (dark/light)\
z - snap to grid\