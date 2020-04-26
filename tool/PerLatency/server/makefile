
SOURCE_PATH  = source
INCLUDE_PATH = include
OBJ_PATH     = obj

CUR_CFLAGS = $(CFLAGS) -I$(INCLUDE_PATH)
CUR_LD_CFLAGS = -lpthread $(LD_CFLAGES)

SOURCE_FILES = $(wildcard $(SOURCE_PATH)/*.c)
OBJ_FILES    = $(addprefix $(OBJ_PATH)/, $(addsuffix .o,$(notdir $(basename $(SOURCE_FILES)))))

TARGET = $(BIN_PATH)/server

.PHONY:all clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(LD) -o $@ $^ $(CUR_LD_CFLAGS)
	
$(OBJ_PATH)/%.o: $(SOURCE_PATH)/%.c
	@if [ ! -d $(OBJ_PATH) ];then mkdir $(OBJ_PATH); fi
	$(CC) $(CUR_CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_PATH) $(TARGET)
