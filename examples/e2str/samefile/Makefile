
EXE = e2str
SRC = main.cpp
RFL = myenum.hpp # files to be reflected
OBJ = $(patsubst %.cpp,.build/%.o,$(SRC))
DEP = $(OBJ:.o=.d)

CXX = clang++
LD = clang++
CXX_FLAGS = -std=c++11 -Wall -MMD
DBG_FLAGS = -g3
LD_FLAGS =
PYPATH = $(PYTHONPATH)

#------------------

.PHONY: all
all: $(EXE) run

.PHONY: run
run:
	./$(EXE)

$(EXE): pygen $(OBJ)
	$(LD) $(LD_FLAGS) $(DBG_FLAGS) $(OBJ) -o $@

$(RFL): codegen.py

pygen: $(RFL)
	python3 codegen.py --gen-code $<

.build:
	if [ ! -d .build ] ; then mkdir .build ; fi

.build/%.o: %.cpp .build
	$(CXX) $(CXX_FLAGS) $(DBG_FLAGS) -c $< -o $@

clean:
	rm -rf .build

# automatic dependencies
-include $(DEP)  # http://stackoverflow.com/a/2908351/5875572

