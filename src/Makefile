# $ make
# $ ./molecular-sim

IDIR = ../include
CC = g++
CPPFLAGS = -g -std=c++11 -I$(IDIR) -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused

O_DIR = obj
L_DIR = ../lib
B_DIR = ../build

EXEC = $(B_DIR)/build

LIBS = -lsfml-graphics -lsfml-window -lsfml-system

_DEPS = main.h
DEPS = $(patsubst %,$(I_DIR)/%,$(_DEPS))

_OBJ = main.o
OBJ = $(patsubst %,$(O_DIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	gcc -o $(EXEC) $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $~ core $(INCDIR)/*~
