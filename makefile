OBJ:=$(patsubst %.c, %.o, $(wildcard *.c))
Result.exe: $(OBJ)
	gcc -o $@ $^
clean:
	rm *.exe *.o