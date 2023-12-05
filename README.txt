Logic Gates extra branch is a sequal to my logic gates project. This adds GUI saving and loading functionality as well as improvements to wire rendering. I didn't merge this with the original since there needs to be different source code for linux vs windows (windows API vs zenity file explorer). I've also written tools for interfacing with these systems.

Compile (windows 64 bit):
gcc logicgatesWindows.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -O3 -o logicgatesWindows.exe

Compile (linux):
gcc logicgatesLinux.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o logicgatesLinux.o

Then run logicgates.exe

I've already included the exe and object files as well, so you don't need to compile if they already work (no guarentees though, this stuff is very finicky)

To load a file, type:
logicgates.exe {filename.txt}
in the terminal (with no brackets)
I've included a seven segment display as well as a 4 bit Arithmetic Logic Unit

Keybinds:
click and drag - Place components, move components, or move screen
click on a POWER component to toggle it on/off
space + click + drag - create wire
1, e, p - POWER component (input/output)
2, n - NOT gate
3, a - AND gate
4, q, o - OR gate
5 - XOR gate
6 - NOR gate
7 - NAND gate
8 - BUFFER gate
x - delete component
shift + click + drag - select
shift + click - add component to selection
c - copy/paste selected
scroll wheel (or up and down arrows) - zoom
up and down arrows - zoom
space + scroll wheel - rotate component (coarse)
sideways arrows - rotate component (fine)
space + sideways arrows - rotate selected (fine)
k - export to file
h - toggle sidebar
w - toggle wireMode
t - toggle theme (dark/light)
z - snap to grid
