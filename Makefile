CC	= gcc
CXX = g++
INC_PATH = .

CFLAGS	= -O2 -g -Wall -Wno-deprecated -std=c++11 -fPIC
CFLAGS 	+= -I$(INC_PATH) $(INCLUDE)
CFLAGS  += ${FLAGS}
LDFLAGS	= -L/usr/lib -lpthread -ldl

# 输出文件名
INDEX = libindex
INDEX_STATIC = ./lib/$(INDEX).a
INDEX_SHARED = ./lib/$(INDEX).so
OUTPUT_PATH = ./obj


#设置VPATH 包含源码的子目录列表
#添加源文件
SUBINC = index
#添加头文件
SUBDIR = index

#设置VPATH
INCLUDE = $(foreach n, $(SUBINC), -I$(INC_PATH)/$(n))
SPACE 	= 
VPATH 	= $(subst $(SPACE),, $(strip $(foreach n,$(SUBDIR), $(INC_PATH)/$(n)))) $(OUTPUT_PATH)

C_SOURCES 	= $(notdir $(foreach n, $(SUBDIR), $(wildcard $(INC_PATH)/$(n)/*.c)))
CPP_SOURCES = $(notdir $(foreach n, $(SUBDIR), $(wildcard $(INC_PATH)/$(n)/*.cpp)))

C_OBJECTS 	= $(patsubst  %.c,  $(OUTPUT_PATH)/%.o, $(C_SOURCES))
CPP_OBJECTS = $(patsubst  %.cpp,  $(OUTPUT_PATH)/%.o, $(CPP_SOURCES))

CXX_SOURCES = $(CPP_SOURCES) $(C_SOURCES)
CXX_OBJECTS = $(CPP_OBJECTS) $(C_OBJECTS)


all: mkdir $(INDEX_SHARED)

$(INDEX_STATIC):$(CXX_OBJECTS)
	ar cr -o $@ $(foreach n, $(CXX_OBJECTS), $(n)) $(foreach n, $(OBJS), $(n))
	#******************************************************************************#
	#                          build static successful !                           #
	#******************************************************************************#

$(INDEX_SHARED):$(CXX_OBJECTS)
	$(CXX) -shared -o $@ $(foreach n, $(CXX_OBJECTS), $(n)) $(foreach n, $(OBJS), $(n))  $(LDFLAGS)
	#******************************************************************************#
	#                          build shared successful !                           #
	#******************************************************************************#

$(OUTPUT_PATH)/%.o:%.cpp
	$(CXX) $< -c $(CFLAGS) -o $@
	
$(OUTPUT_PATH)/%.o:%.c
	$(CC) $< -c $(CFLAGS) -o $@

mkdir:
	mkdir -p $(dir $(INDEX_SHARED))
	mkdir -p $(OUTPUT_PATH)
	
rmdir:
	rm -rf $(dir $(INDEX_SHARED))
	rm -rf $(OUTPUT_PATH)

clean:
	rm -f $(OUTPUT_PATH)/*
	rm -rf $(INDEX_SHARED) ${INDEX_SHARED}

install:
	find index -name *.h | xargs -I{} cp --parents {} /usr/local/include/
	cp -rf ${INDEX_SHARED} /usr/local/lib64

