TARGET = server

$(TARGET) : obj/main.o obj/file.o
	gcc obj/main.o obj/file.o -o server

obj/main.o : src/main.c
	gcc -c src/main.c -o obj/main.o

obj/file.o : src/file.c
	gcc -c src/file.c -o obj/file.o

clean :
	rm server obj/*.o
