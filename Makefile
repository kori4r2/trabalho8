CFLAGS = -g -Wall -lm

all: build

build:
	gcc -o program *.c -I./ $(CFLAGS)

clean:
	rm -f *.o
	rm -f *.dif
	rm -f *.zip
	rm -f test*.out
	rm -f program
	clear

run: build
	./program

.zip:
	zip trabalho8.zip *.c *.h Makefile

debug: build
	valgrind --log-file=log.txt --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 1.in > test1.out 2> error.out
	diff 1.out test1.out > 1.dif
	valgrind --log-file=log.txt --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 2.in > test2.out 2> error.out
	diff 2.out test2.out > 2.dif
	valgrind --log-file=log.txt --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 3.in > test3.out 2> error.out
	diff 3.out test3.out > 3.dif
	valgrind --log-file=log.txt --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 4.in > test4.out 2> error.out
	diff 4.out test4.out > 4.dif
	valgrind --log-file=log.txt --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 5.in > test5.out 2> error.out
	diff 5.out test5.out > 5.dif
	valgrind --log-file=log.txt --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 6.in > test6.out 2> error.out
	diff 6.out test6.out > 6.dif
	valgrind --log-file=log.txt --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 7.in > test7.out 2> error.out
	diff 7.out test7.out > 7.dif
	#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 8.in > test8.out
	#diff 8.out test8.out > 8.dif
	#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 9.in > test9.out
	#diff 9.out test9.out > 9.dif
	#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program < 10.in > test10.out
	#diff 10.out test10.out > 10.dif

