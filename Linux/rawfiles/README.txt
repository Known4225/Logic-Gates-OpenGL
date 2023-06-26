<<<<<<< HEAD
If the static libraries in the "Linux" folder (libglad.a and libglfw3.a) didn't work then you'll have to compile them yourself.

Using cmake, navigate terminal to the "rawfiles" directory, then type:

cmake -S ./glfw-3.3.8/ -B ./glfw-3.3.8/build
cd glfw-3.3.8/
cd build
make

once it's done, go to the src folder (in build) and move the libglfw3.a file into the "Linux" folder (replacing the old one)

to get the libglad.a file, navigate back to the "rawfiles" directory, then type:

gcc glad.c -c -I./ -L./ -lglfw3 -lopengl32 -lgdi32 -lglad -o glad.o
ar rcs libglad.a glad.o

then take the libglad.a file and move it into the "Linux" folder (replacing the old one)

then you should be good, if not then you're probably using Arch or something
=======
If the static libraries in the "Linux" folder (libglad.a and libglfw3.a) didn't work then you'll have to compile them yourself.

Using cmake, navigate terminal to the "rawfiles" directory, then type:

cmake -S ./glfw-3.3.8/ -B ./glfw-3.3.8/build
cd glfw-3.3.8/
cd build
make

once it's done, go to the src folder (in build) and move the libglfw3.a file into the "Linux" folder (replacing the old one)

to get the libglad.a file, navigate back to the "rawfiles" directory, then type:

gcc glad.c -c -I./ -L./ -lglfw3 -lopengl32 -lgdi32 -lglad -o glad.o
ar rcs libglad.a glad.o

then take the libglad.a file and move it into the "Linux" folder (replacing the old one)

then you should be good, if not then you're probably using Arch or something
>>>>>>> a141004eaccfa4b93286fc37862031d85407811b
You're on linux, figure it out