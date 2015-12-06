CFLAGS = -g -Wall -lm

all: build

build:
	gcc -o program *.c -I./ $(CFLAGS)

clean:
	rm -f *.o
	rm -f *.zip
	rm -f test*.out
	rm -f program
	clear

run: build
	./program

.zip:
	zip trabalho7.zip *.c *.h Makefile

debug: build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 1.in > test1.out
	diff 1.out test1.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 2.in > test2.out
	diff 2.out test2.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 3.in > test3.out
	diff 3.out test3.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 4.in > test4.out
	diff 4.out test4.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 5.in > test5.out
	diff 5.out test5.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 6.in > test6.out
	diff 6.out test6.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 7.in > test7.out
	diff 7.out test7.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 8.in > test8.out
	diff 8.out test8.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 9.in > test9.out
	diff 9.out test9.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 10.in > test10.out
	diff 10.out test10.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 11.in > test11.out
	diff 11.out test11.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 12.in > test12.out
	diff 12.out test12.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 13.in > test13.out
	diff 13.out test13.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 14.in > test14.out
	diff 14.out test14.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 15.in > test15.out
	diff 15.out test15.out

