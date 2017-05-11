
EXE = e2str
SRC = main.cpp
RFL = myenum.hpp # files to be reflected

PYTHON3 = env PYTHONPATH=$(PYTHONPATH) python3
CXX = clang++
LD = clang++
CXX_FLAGS = -std=c++11 -Wall -MMD
DBG_FLAGS = -g3
LD_FLAGS =

#------------------
GENH := $(shell $(PYTHON3) codegen.py --show-out-hdr $(RFL)) # headers generated from the reflected files
GENC := $(shell $(PYTHON3) codegen.py --show-out-src $(RFL)) # sources generated from the reflected files
OBJ = $(patsubst %.cpp,.build/%.o,$(SRC) $(GENC))
DEP = $(OBJ:.o=.d)

#------------------

.PHONY: all
all: $(EXE) run

.PHONY: run
run:
	./$(EXE)

$(EXE): pygen $(OBJ)
	$(LD) $(LD_FLAGS) $(DBG_FLAGS) $(OBJ) -o $@

.build:
	if [ ! -d .build ] ; then mkdir .build ; fi

.build/%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(DBG_FLAGS) -c $< -o $@

.PHONY: pygen
pygen: $(GENH) $(GENC)

%.gen.cpp %.gen.hpp: %.hpp codegen.py
	$(PYTHON3) codegen.py --gen-code $<

clean:
	rm *.o

# automatic dependencies
-include $(DEP)  # http://stackoverflow.com/a/2908351/5875572

