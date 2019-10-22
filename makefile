CC= g++
### Directories
DEBUG = ./Debug/
Json = ./src/Json/
Com = ./src/Com/
RtCaster = ./src/RtCaster/
Rtklib =./src/Rtklib/

### Patterns 
C_SRC= $(wildcard *.cpp $(Json)/*.cpp  $(Com)/*.cpp $(RtCaster)/*.cpp $(Rtklib)/*.cpp)
C_OBJ= $(patsubst %.cpp,%.o,$(C_SRC))
### TMP OBJECT
TMP_OBJ= $(patsubst %.cpp,%.o,$(notdir $(C_SRC)))
F_OBJ= $(patsubst %.o,$(DEBUG)%.o,$(TMP_OBJ))

### Target
TARGET=VrsCaster
.PHONY: all clean
all:$(TARGET)
### Compiler Flags
LCFLAGS= -g -c -std=c++11 
#######################################################################################
$(TARGET):$(C_OBJ)
	$(CC) -o $(TARGET) $(TMP_OBJ)  -lpthread -lm
	mv $(TMP_OBJ) $(DEBUG) 
%.o:%.cpp
	$(CC) $(LCFLAGS) $<
clean:
	rm $(F_OBJ) $(TARGET) $(TMP_OBJ)
