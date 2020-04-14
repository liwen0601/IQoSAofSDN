######################################################################
#  Author: Wen Li 
#  Date: 4/5/2020
# Description: dpi module
######################################################################

SOURCE_DIR = source
HEADER_DIR = include
OBJ_PATH = obj

CUR_INCLUDES = -I$(HEADER_DIR) -I$(LIB_PATH)/include
CUR_CFLAGS = $(CFLAGS)
#$(LIB_PATH)/libdpi.so 
CUR_LD_CFLAGES = $(LD_CFLAGES) -lpcap 

SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)
OBJ_FILES = $(addprefix $(OBJ_PATH)/, $(addsuffix .o,$(notdir $(basename $(SOURCE_FILES)))))

TARGET = dpiDeamon

.PHONY:all clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(LD) $(CUR_LD_CFLAGES) -o $@ $^ 
	
$(OBJ_PATH)/%.o: $(SOURCE_DIR)/%.cpp
	@if [ ! -d $(OBJ_PATH) ];then mkdir $(OBJ_PATH); fi
	$(CC) $(CUR_CFLAGS) -o $@ $(CUR_INCLUDES) -c $<

clean:
	rm -rf $(OBJ_PATH) $(TARGET)


