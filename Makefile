# ==================================================
# Temperature Retrieval
# ==================================================

TARGET = temperature-retrieval

CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3

# Include directories
INC_DIR = inc $(HOME)/include
INC_PATHS = $(addprefix -I, $(INC_DIR))

# Library directories
LIB_DIR = $(HOME)/lib
LIB_PATHS = $(addprefix -L, $(LIB_DIR))

# Libraries (add when needed)
LIB_OPTS = -lm

# Object files
OBJS = \
	main.o \
	Nair_to_T_p_hydrostatic.o \
	Geocoordinate.o \
	LookingDirection.o \
	across_point_atmosphere.o \
	cross_point_at_altitude.o \
	sensor_direction.o \
	solar_direction.o \
	translate_coordinate.o\
	to_sasktran2_geometry.o

OBJ_PATHS = $(addprefix obj/, $(OBJS))

# Source directories
vpath %.cpp src:src/Atmosphere:src/coordinate:src/Sasktran2Geometry

# ==================================================
# Build
# ==================================================

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ_PATHS)
	$(CXX) $(OBJ_PATHS) -o $@ $(LIB_PATHS) $(LIB_OPTS)

obj/%.o: %.cpp
	@mkdir -p obj
	$(CXX) -c $< $(CXXFLAGS) -MMD -MP $(INC_PATHS) -o $@

# Header dependencies
-include $(OBJ_PATHS:.o=.d)

# Clean
clean:
	rm -f $(OBJ_PATHS) $(OBJ_PATHS:.o=.d) $(TARGET)
