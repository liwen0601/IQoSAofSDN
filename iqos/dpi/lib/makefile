######################################################################
#  Author: Wen Li 
#  Date: 4/5/2020
# Description: dpi module
######################################################################

SOURCE_DIR = source
HEADER_DIR = include
OBJ_PATH = obj

CUR_INCLUDES = -I$(HEADER_DIR)
CUR_CFLAGS = $(CFLAGS) -fpic

SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)
OBJ_FILES = $(addprefix $(OBJ_PATH)/, $(addsuffix .o,$(notdir $(basename $(SOURCE_FILES)))))

TARGET = libdpi.so

.PHONY:all clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(LD) -shared $(LD_CFLAGES) -o $@ $^ 
	
$(OBJ_PATH)/%.o: $(SOURCE_DIR)/%.cpp
	@if [ ! -d $(OBJ_PATH) ];then mkdir $(OBJ_PATH); fi
	$(CC) $(CUR_CFLAGS) -o $@ $(CUR_INCLUDES) -c $<

clean:
	rm -rf $(OBJ_PATH) $(TARGET)


