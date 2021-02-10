CXX = g++
CXXFLAGS = -pedantic -std=c++17
LDFLAGS = 
EXECUTABLE = my_editor
SRC = main.cpp file_hdl.cpp document.cpp editor.cpp args_hdl.cpp reader.cpp
OBJ = $(SRC:.cpp=.o)

$(EXECUTABLE) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(EXECUTABLE) $(LDFLAGS)
	
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f *.o $(EXECUTABLE)