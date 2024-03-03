all:
	gcc logicgates.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -DOS_LINUX -o logicgates.o
win:
	gcc logicgates.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -DOS_WINDOWS -o logicgates.exe
val:
	gcc logicgates.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -g -DOS_LINUX -o logicgates.o
clean:
	rm logicgates.o