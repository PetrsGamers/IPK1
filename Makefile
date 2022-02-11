FLAGS=-std=c99 -Wall -Wextra
all: hinfosvc.c
    gcc hinfosvc.c ${FLAGS} -o hinfosvc