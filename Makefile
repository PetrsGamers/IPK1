FLAGS = -std=c99 -Wall -Wextra
all:hinfosvc.c
	gcc ${FLAGS} hinfosvc.c -o hinfosvc