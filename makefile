# CS 370
# Simple make file for project #5A

CFLAGS	= -ggdb -O0 -pthread

evilNums:	evilNums.c
	gcc $(CFLAGS) -o evilNums evilNums.c

clean:
	@rm -f evilNums

