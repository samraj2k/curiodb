CXX = clang++
CXXFLAGS = -std=c++20 -stdlib=libc++ -Wall -g
LDFLAGS =

# Source files
SRC = main.cpp inmemorybtreeutils.cpp

# Object files
OBJ = $(SRC:.cpp=.o)


# Target executable
TARGET = main
FINAL = ./main

# Build rule
all: $(TARGET)

$(TARGET): $(OBJ)
	echo ${OBJ}
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJ)

# Compile rule
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJ) $(TARGET)

run:
	./main
# Dependency rule
# Uncomment and edit if needed, for example:
# main.o: btreeutils.h inmemorybtreeutils.h