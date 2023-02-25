edit: canvas.c canvas.h camera.o ray.o world.o voxel.o outfuncs.o charbuffer.o loadfuncs.o movefuncs.o globals.o 3d.h
	gcc canvas.c canvas.h ray.o world.o voxel.o camera.o charbuffer.o loadfuncs.o movefuncs.o outfuncs.o globals.o -o edit -lm -ltermbox
camera: camera.c camera.h globals.h
	gcc -c camera.c
ray.o: ray.c ray.h
	gcc -c ray.c
world.o: world.c world.h
	gcc -c world.c
voxel.o: voxel.c voxel.h
	gcc -c voxel.c
charbuffer.o: charbuffer.c charbuffer.h
	gcc -c charbuffer.c
loadfuncs.o: loadfuncs.c loadfuncs.h movefuncs.c movefuncs.h charbuffer.c
	gcc -UCOMPILE_CHANGE -c loadfuncs.c -o loadfuncs.o
loadfuncs_change.o: loadfuncs.c loadfuncs.h movefuncs.c movefuncs.h charbuffer.c
	gcc -DCOMPILE_CHANGE -c loadfuncs.c -o loadfuncs_change.o
movefuncs.o: loadfuncs.c loadfuncs.h movefuncs.c movefuncs.h charbuffer.c
	gcc -UCOMPILE_CHANGE -c movefuncs.c
movefuncs_change.o: loadfuncs.c loadfuncs.h movefuncs.c movefuncs.h charbuffer.c
	gcc -DCOMPILE_CHANGE -c movefuncs.c -o movefuncs_change.o
outfuncs.o: outfuncs.c outfuncs.h loadfuncs.c loadfuncs.h charbuffer.c charbuffer.h movefuncs.c movefuncs.h
	gcc -c outfuncs.c
globals.o: globals.c globals.h
	gcc -c globals.c globals.h
