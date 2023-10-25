CC = gcc
LD = gcc

PROJ := bforcesha

SRC := src/main.c \
	src/stringTools.c \
	src/rainbow.c \
	src/hashTable.c \
	src/linkedList.c

INCLUDE := -Iinclude

CFLAGS := -O2 -Wall
LIBS := -lcrypto -lpthread

all:
	$(CC) -o $(PROJ) $(CFLAGS) $(SRC) $(INCLUDE) $(LIBS)

run:
	@./$(PROJ)