CC = gcc
LD = gcc

PROJ := stormbreaker

SRC := src/main.c \
	src/stringTools.c \
	src/rainbow.c \
	src/hashTable.c \
	src/linkedList.c

INCLUDE := -Iinclude

CFLAGS := -O2 -Wall
LIBS := -lcrypto -lpthread

build:
	$(CC) -o $(PROJ) $(CFLAGS) $(SRC) $(INCLUDE) $(LIBS)

clean:
	rm -rf $(PROJ)

run:
	@./$(PROJ)

docker-build:
	docker build -t $(PROJ):latest .

docker-export:
	docker save $(PROJ):latest > $(PROJ)-latest.tar

docker-run:
	docker run --rm -it -v $(CURDIR):$(CURDIR) -w $(CURDIR) $(PROJ) /bin/bash