# $ make
# $ ./molecular-sim

PROG = molecular-sim
I_DIR = ../include
CC = clang++
CPPFLAGS = -g -std=gnu++11 -stdlib=libc++ -I$(I_DIR) -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlong-long -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-overflow=5 -Wundef -Werror -Wno-unused -Wno-long-long

O_DIR = obj
L_DIR = ../lib
B_DIR = ../build

EXEC = $(B_DIR)/$(PROG)

LIBS = -lsfml-graphics -lsfml-window -lsfml-system

_DEPS = main.h particle.h event.h collisionSystem.h
DEPS = $(patsubst %,$(I_DIR)/%,$(_DEPS))

_OBJS = main.o particle.o event.o collisionSystem.o
OBJS = $(patsubst %,$(O_DIR)/%,$(_OBJS))

$(O_DIR)/%.o: %.cc $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

main: $(OBJS)
	$(CC) -o $(EXEC) $^ $(CPPFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(O_DIR)/*.o
