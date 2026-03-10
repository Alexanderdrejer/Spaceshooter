CXX = g++

CXXFLAGS = -g -Wall -O2

SRC = src

INC = inc

BUILD = build
BIN = bin
BIN_PATH = $(BUILD)/$(BIN)

TGT1 = blkio_edge
TGT2 = blkio_lvl
TGT3 = nblkio_edge


all: $(TGT1).out \
     $(TGT2).out \
     $(TGT3).out

install:
	@echo 'Creating build directory $(BIN_PATH)...'
	@mkdir -p $(BUILD)
	@mkdir -p $(BIN_PATH)
	@echo 'Moving binary file(s) to $(BIN_PATH)...'
	@mv *.out $(BIN_PATH)
	@echo 'Done.'

%.out: $(SRC)/%.cpp

	$(CXX) $(CXXFLAGS) $^ -o $@


$(TGT1).out: $(SRC)/$(TGT1).cpp

	$(CXX) $(CXXFLAGS) $^ -o $@

$(TGT2).out: $(SRC)/$(TGT2).cpp

	$(CXX) $(CXXFLAGS) $^ -o $@

$(TGT3).out: $(SRC)/$(TGT3).cpp

	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f *.out
	rm -r $(BUILD)
