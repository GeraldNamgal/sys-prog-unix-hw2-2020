# Gerald Arocena
# CSCI E-20, Spring 2020
# 
# makefile for dulite.c
#

CC     = cc
CFLAGS = -Wall -Wextra -g 

ls1: ls1.c
	$(CC) $(CFLAGS) -o ls1 ls1.c

ls2: ls2.c
	$(CC) $(CFLAGS) -o ls2 ls2.c

dulite: dulite.c
	$(CC) $(CFLAGS) -o dulite dulite.c

clean:
	rm -f ls1 ls2 dulite ls1.o ls2.o dulite.o 

