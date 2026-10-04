# 1. Variables: Define compiler and standard
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# 2. Define the output executable name
TARGET = expense_tracker

# 3. List your source files
SRCS = main.cpp ExpenseManager.cpp User.cpp

# 4. Define object files (compiled versions of your code)
OBJS = $(SRCS:.cpp=.o)

# 5. Default target: Compile and run
all: $(TARGET)
	./$(TARGET)

# 6. Linking: Combine objects into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# 7. Pattern rule: Compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 8. Clean target: Remove compiled files
clean:
	rm -f $(OBJS) $(TARGET)