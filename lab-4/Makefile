all:
	gcc ./src/libs/lib1.c ./src/libs/utilities.c ./src/static.c -o ./staticLib1.out
	gcc ./src/libs/lib2.c ./src/libs/utilities.c ./src/static.c -o ./staticLib2.out
	gcc -fPIC -shared ./src/libs/lib1.c -o lib1.so
	gcc -fPIC -shared ./src/libs/lib2.c -o lib2.so
	gcc ./src/libs/utilities.c ./src/dynamic.c -ldl -o ./dynamic.out
