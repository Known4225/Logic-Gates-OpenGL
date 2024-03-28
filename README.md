# Logic Gates

### Compile (windows 64 bit):
gcc logicgates.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -DOS_WINDOWS -o logicgates.exe\
Then run logicgates.exe\
I've also included the 64-bit binary (logicgates.exe) for convenience,\
so you can just run that without compilation if you're on windows (and on a 64 bit machine)

### Compile (linux):
gcc logicgates.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -DOS_LINUX -o logicgates.o\
Then run ./logicgates.o

To load a file, type:\
logicgates.exe {filename.txt}\
in the terminal (with no brackets)\
I've included some demo projects in the "Demos" folder\
Files can also be loaded in via the GUI menu

This application is designed to be usable with just a mouse, but there are\
keybind shortcuts to make building circuits faster.

## Keybinds:
* click and drag - Place components, move components, or move screen
* click on a POWER component to toggle it on/off
* scroll wheel - zoom
* space + click + drag - create wire
* 1, e, p - POWER component (input/output)
* 2, n - NOT gate
* 3, a - AND gate
* 4, q, o - OR gate
* 5 - XOR gate
* 6 - NOR gate
* 7 - NAND gate
* 8 - BUFFER gate
* x - delete component
* shift + click + drag - select
* shift + click - add component to selection
* c - copy/paste selected
* scroll wheel (or up and down arrows) - zoom
* up and down arrows - zoom (fine)
* space/shift + scroll wheel - rotate component (coarse)
* sideways arrows - rotate component (fine)
* space + sideways arrows - rotate selected (fine)
* k - export to file
* h - toggle sidebar
* w - toggle wireMode
* t - toggle theme (dark/light)
* z - snap to grid
* ctrl + c - copy
* ctrl + x - cut
* ctrl + v - paste
* ctrl + z - undo
* ctrl + y - redo
* shift + x - delete and replace

## Debugger
Press d to toggle the debugger on/off - you should see a flash when you do this and an orange square appears in the top left when the debugger is on\
In the debugger, the following keybinds are available:
* ctrl + space - step one tick
* ctrl + scroll - step one tick (forwards or backwards)

Note that the backwards step is a simple undo. Meaning any edits you make while the debugger is running can be rolled back if you scroll back\
It is not recommended to make lots of edits in debug mode if you are using the backwards step feature. It is intended for precise viewing of tick-by-tick sequences


### Personal Feature Request:
 - Debugger (step through tick by tick)
 - Grid mode
 - Grouped units "summonable" (without having to save and add file)
 - Multiple tabs
 - Multiple colours? (customisable?)
 
