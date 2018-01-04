EXECNAME?=main

VERSION:=$(shell cat version.txt)

CXXFLAGS+=-Wall -Wextra -std=gnu++11 -pedantic -fPIC -D_PROJECT_VERSION=$(VERSION) \
-Wno-unused

LIBS+=

LDFLAGS+=

LD=$(CXX)
RM=rm

OBJS=src/main.o \
src/core/core.o \
src/graphics/graphics.o \


SO_LIBS+=-lIrrlicht
LIBS+=$(SO_LIBS)

INCLUDES=-I/usr/include/irrlicht -Iinclude
CXXFLAGS+=$(INCLUDES)

# ----------------------------------------------------------------------------

.PHONY: all clean run ee bee doc

all: $(EXECNAME)

clean:
	$(RM) -fv $(OBJS) $(EXECNAME)

cleandoc:
	$(RM) -frv doxygen-out

cleanall: clean cleandoc

run: $(EXECNAME)
	./$(EXECNAME)

doc: doxygen-config.txt
	doxygen doxygen-config.txt

# Debug .PHONY targets
ee: clean run
bee: clean all

# Not .PHONY targets
$(EXECNAME): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(EXECNAME)
