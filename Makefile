CFLAGS += -Wall -pedantic -std=c99
CC ?= clang
BIN_NAME ?= cottage
SRCS = cottage.c 

all: cottage
cottage: cottage.o

debug: CFLAGS += -O0 -g -DDEBUG
debug: all

clean:
	@rm -f ./*.o
	@rm -f $(BIN_NAME)

install:
	@install -m 0755 $(BIN_NAME) /usr/bin

check:
	@./checkpatch.pl --no-tree --ignore LONG_LINE,NEW_TYPEDEFS,UNNECESSARY_ELSE,MACRO_WITH_FLOW_CONTROL -f cottage.c

analyse:
	@scan-build -v -o analyse make debug
