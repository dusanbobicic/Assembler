CC = g++
IDIR = ./src
CFLAGS = -Wall -I$(IDIR) -std=c++17
PROGRAM = ./bin/comp
SRCDIR = ./src
OBJDIR = ./obj

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ= $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o, $(SRC))

$(PROGRAM): $(OBJ)
	$(CC) -std=c++17 -g -o $@ $^
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c -MMD -o $@ $< $(CFLAGS)

-include $(OBJDIR)/*.d

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(OBJDIR)/*.o
	rm -f $(PROGRAM)

.PHONY: clean

