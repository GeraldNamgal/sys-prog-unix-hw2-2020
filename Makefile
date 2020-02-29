# Gerald Arocena
# CSCI E-20, Spring 2020
# 
# makefile for dulite.c
#

CC     = cc
CFLAGS = -Wall -Wextra -g 

dulite: dulite.c
	$(CC) $(CFLAGS) -o dulite dulite.c

clean:
	rm -f dulite dulite.o 

