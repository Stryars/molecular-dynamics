# $ make
# $ ./molecular-sim

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	CXX = g++
	CXXFLAGS := -g -std=c++11 -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlong-long -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-overflow=5 -Wundef -Werror -Wno-unused -Wno-long-long
endif
ifeq ($(UNAME_S), Darwin)
	CXX = /usr/local/Cellar/llvm/6.0.0/bin/clang++
	CXXFLAGS := -g -std=c++11 -stdlib=libc++ -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlong-long -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-overflow=5 -Wundef -Werror -Wno-unused -Wno-long-long
endif
SRCDIR := src
BUILDDIR := build
TARGET := bin/mdsim

SRCEXT := cc
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
LIB := -L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system
INC := -I.

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CXX) $^ -o $(TARGET) $(LIB)"; $(CXX) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<"; $(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
