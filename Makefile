CXX = g++
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -pthread
TARGET = scanner

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp $(LDFLAGS)

clean:
	rm -f $(TARGET)