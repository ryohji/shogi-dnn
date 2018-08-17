.PHONY: clean

shogi_test: move.o shogi.h shogi.c
	$(CC) -o $@ -DUNITTEST_ move.o shogi.c

move_test: filter.o move.h move.c
	$(CC) -o $@ -DUNITTEST_ filter.o move.c

clean:
	$(RM) *.o *~ shogi_test move_test

shogi.o: shogi.h move.h shogi.c
move.o: move.h move.c
filter.o: filter.h filter.c
