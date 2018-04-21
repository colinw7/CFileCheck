C_CC   = gcc
CPP_CC = g++

RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: dirs $(BIN_DIR)/CFileCheck

dirs:
	@if [ ! -e ../obj ]; then mkdir ../obj; fi
	@if [ ! -e ../bin ]; then mkdir ../bin; fi

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CFileCheck

CPP_SRC = \
CFileCheck.cpp \

CPP_OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SRC))

CPPFLAGS = \
-std=c++14 \
-I$(INC_DIR) \
-I../../CFile/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I.

LFLAGS = \
$(LEBUG) \
-L$(LIB_DIR) \
-L../../CFile/lib \
-L../../CStrUtil/lib \
-L../../COS/lib \
-lCFile \
-lCStrUtil \
-lCOS \

.SUFFIXES: .cpp

$(CPP_OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CPP_CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CFileCheck: $(CPP_OBJS) $(C_OBJS)
	$(CPP_CC) -o $(BIN_DIR)/CFileCheck $(CPP_OBJS) $(C_OBJS) $(LFLAGS)
