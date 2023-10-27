CC = gcc
LD = gcc

PROJ := stormbreaker

SRC := src/main.c \
	src/hashTable.c \
	src/linkedList.c \
	src/outFileBuffer.c \
	src/inFileBuffer.c \
	src/rainbowCreate.c \
	src/rainbowLoad.c \
	src/rainbowSolve.c \

CFLAGS := -O2 -Wall
LIBS := -lcrypto -lpthread

build:
	$(CC) -o $(PROJ) $(CFLAGS) $(SRC) $(LIBS)
	chmod a+x $(PROJ)

clean:
	rm -rf $(PROJ)

docker-build:
	docker build -t $(PROJ):latest .

docker-export:
	docker save $(PROJ):latest > $(PROJ)-latest.tar

docker-run:
	docker run --rm -it -v $(CURDIR):$(CURDIR) -w $(CURDIR) $(PROJ) /bin/bash