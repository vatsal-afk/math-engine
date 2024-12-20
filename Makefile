# Compiler and flags
CXX = emcc
CXXFLAGS = -std=c++17 -Wall -I./src -I/home/virtual/Downloads/emsdk/upstream/emscripten/system/include -s USE_GLFW=3 -s FULL_ES3=1 -s WASM=1 -s USE_WEBGL2=1 -s FETCH=1

# Output target
TARGET = window.html

# Directories
SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:.cpp=.o)

# Build target
$(TARGET): $(SRCS)
	$(CXX) $(SRCS) -o $(TARGET) $(CXXFLAGS)

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)
