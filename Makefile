OUTPUT=main
#VPATH=src
CFLAGS+=$(shell pkg-config --cflags json-c)
LDFLAGS+=$(shell pkg-config --libs json-c)
CFILES=$(wildcard *.c)
OBJS=$(CFILES:.c=.o)

%.o: %.c *.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OUTPUT): $(OBJS)
	$(CC) $(LDFLAGS) -o $(OUTPUT) $(OBJS)

all: $(OUTPUT)

.PHONY: clean

clean:
	rm -f *.o $(OUTPUT)

