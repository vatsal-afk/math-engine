CXX = g++
CXXFLAGS = -lglfw -lGL -lm -lGLEW
TARGET = window
SRC = main.cpp

$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS)

clean:
	rm -f $(TARGET)
