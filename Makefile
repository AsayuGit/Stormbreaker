CC = gcc
LD = gcc

PROJ := bforcesha

SRC := src/main.c \
	src/hash.c \
	src/bruteforce.c \
	src/filemanager.c \
	src/rainbow.c

INCLUDE := -Iinclude

CFLAGS := -g -Wall
LIBS := -lcrypto -lpthread

all:
	$(CC) -o $(PROJ) $(CFLAGS) $(SRC) $(INCLUDE) $(LIBS)

run:
	@./$(PROJ)