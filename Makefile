all:
	gcc -I src/include -L src/lib -o raytracing raytracing.c -lmingw32 -lSDL3