CC	= g++
LD	= g++

TOPDIR = .
SRC_DIR = $(TOPDIR)
OBJ_DIR = $(TOPDIR)

CCFLAGS = -O3 `root-config --cflags` -Wall -I$(PEV_INC)
LDFLAGS = -O3 `root-config --libs` -Wall -L$(OBJ_DIR) -I$(PEV_LIB)

PEV_DIR = $(TOPDIR)/../Particle_Event_GENBOD
PEV_LIB = $(PEV_DIR)/lib
PEV_SRC = $(PEV_DIR)/src
PEV_INC = $(PEV_DIR)/inc

PROGRAM = urqmd_parser cleaner

SOURCES := $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
PEV_OBJECTS = $(PEV_LIB)/Particle.o $(PEV_LIB)/Event.o $(PEV_LIB)/ParticleTree.o $(PEV_LIB)/Dict.o

all: $(PROGRAM)

urqmd_parser: urqmd_parser.o $(PEV_OBJECTS)
	$(LD) -o $@ $^ $(LDFLAGS)

urqmd_parser.o: parser.cpp
	$(CC) -c $(CCFLAGS) $< -o $@ 

cleaner: cleaner.cpp
	$(CC) -Wall -O3 -o $@ $<

$(PEV_OBJECTS):
	@echo "No base libs. Create them"

clean:
	@rm -rf $(PROGRAM) *.o
