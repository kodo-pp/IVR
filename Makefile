EXECNAME?=main

VERSION:=$(shell cat version.txt)

CXXFLAGS+=-Wall -Wextra -std=gnu++11 -pedantic -fPIC -D_PROJECT_VERSION=$(VERSION)

LIBS+=

LDFLAGS+=

LD=$(CXX)
RM=rm

OBJS=src/main.o \
src/core/core.o \


SO_LIBS+=-lIrrlicht
LIBS+=$(SO_LIBS)

INCLUDES=-I/usr/include/irrlicht -Iinclude
CXXFLAGS+=$(INCLUDES)

# ----------------------------------------------------------------------------

.PHONY: all clean run ee bee

all: $(EXECNAME)

clean:
	$(RM) -fv $(OBJS) $(EXECNAME)

run: $(EXECNAME)
	./$(EXECNAME)

# Debug .PHONY targets
ee: clean run
bee: clean all

# Not .PHONY targets
$(EXECNAME): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(EXECNAME)
