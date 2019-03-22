#makefile

SOURCE  = myfsinfo.c functions.c utils.c list.c
OBJS    = myfsinfo.o functions.o utils.o list.o
OUT     = myfsinfo
CC      = gcc
FLAGS   = -g -c

#create/compile the individual files seperately
all: $(OUT)

$(OUT): $(OBJS)
     $(CC) -o $(OUT) $(OBJS) 

functions.o: functions.c functions.h
    $(CC) $(FLAGS) functions.c

myfsinfo.o: myfsinfo.c functions.h
    $(CC) $(FLAGS) myfsinfo.c

utils.o: utils.c utils.h
    $(CC) $(FLAGS) utils.c

list.o: list.c list.h
    $(CC) $(FLAGS) list.c

#clean
.PHONY: clean

clean:
    rm -f $(OBJS) myfsinfo