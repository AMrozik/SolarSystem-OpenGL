CXX=g++
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
CFLAGS=-c -Wall -DDEBUG -g3 -fpermissive -MMD

ifeq ($(OS),Windows_NT)
LIBS =-lfreeglut -l:glew32.dll -lopengl32 -lm
EXECUTABLE = SolarSystem.exe
else
LIBS =-lX11 -lglut -lGL -lGLU -lGLEW -lm
EXECUTABLE = SolarSystem
endif

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
	rm -f $(SOURCES:%.cpp=%.d)

# compilation
%.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@ $(LIBS)

# linking
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

run: $(EXECUTABLE)
	./$(EXECUTABLE) -sync -gldebug

-include $(SOURCES:%.cpp=%.d)

zip: clean
	zip -r SolarSystem .

ok: run
	find . -type f -not -name '*.zip' -print0 | xargs -0 rm --
	ok2
