# Makefile for objects associated with the spatial object class

%.o: $(RHSEG_DIR)/spatial/%.cc
    $(CPP) -c $(CPPFLAGS) $(DEFINES) $(input) -o $(output)

OBJECTS := spatial.o trace_region_boundary.o

all: $(OBJECTS)

