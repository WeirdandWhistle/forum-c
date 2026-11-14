TARGET=main
CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter -fsanitize=address
SRC=src
LIB=lib

CFILES=$(shell (find src -name '*.c'))
OFILES=$(subst .c,.o,$(CFILES))

LIBOFILES=$(shell (find lib -name '*.o'))

$(TARGET): $(LIB)/sqlite3.o $(LIB)/mongoose.o $(OFILES) 
	$(CC) $(CFLAGS) -o $(TARGET) $(OFILES) $(LIBOFILES)

$(LIB)/sqlite3.c : pull-deps
$(LIB)/mongoose.c : pull-deps

.c :
	$(shell (echo '.c!'))

%.o : %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

pull-deps:
	cd $(LIB) && sh pull_deps.sh
clean: 
	rm -f $(OFILES) $(TARGET)
clean-deps:
	rm -f $(LIBOFILES)
full-clean: clean clean-deps
	rm -f $(LIB)/mongoose.c $(LIB)/mongoose.h $(LIB)/sqlite3.c $(LIB)/sqlite3.h

run: $(TARGET)
	./$(TARGET)