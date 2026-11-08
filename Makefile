TARGET=main
CC=gcc
CFLAGS=-Wall -Wextra
SRC=src
LIB=lib

CFILES=$(shell (find src -name '*.c'))
OFILES=$(subst .c,.o,$(CFILES))

LIBOFILES=$(shell (find lib -name '*.o'))

$(TARGET): $(LIB)/sqlite3.o $(LIB)/mongoose.o $(OFILES) 
	$(CC) $(CFLAGS) -o $(TARGET) $(OFILES) $(LIBOFILES)


# sqlite3.o : pull-depss
# 	$(CC) $(CFLAGS) -c -o sqlite3.o sqlite3.c
# mongoose.o : pull-deps
# 	$(CC) $(CFLAGS) -c -o mongoose.o mongoose.c

$(LIB)/sqlite3.c : pull-deps
$(LIB)/mongoose.c : pull-deps

.c :
	$(shell (echo '.c!'))

%.o : %.c %.h
# 	$(shell (echo '.o!'))
	$(CC) $(CFLAGS) -c -o $@ $<

pull-deps:
	cd $(LIB) && sh pull_deps.sh
clean: 
	rm -f $(OFILES) $(TARGET)
clean-deps:
	rm -f $(LIBOFILES)
full-clean: clean clean-deps
	rm -f $(LIB)/mongoose.c $(LIB)/mongoose.h $(LIB)/sqlite3.c $(LIB)/sqlite3.h


