all:
	gcc logicgatesLinux.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -o logicgatesLinux.o
win:
	gcc logicgatesWindows.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -o logicgatesWindows.exe
val:
	gcc logicgatesLinux.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -g -o logicgatesLinux.o
clean:
	rm logicgatesLinux.o