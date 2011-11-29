.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

CMN_INC_DIR = $(CODE_DIR)/cmn
CMN_SRC_DIR = $(CODE_DIR)/cmn

MEM_CODE_DIR = $(CODE_DIR)/mem
MEM_INC_DIR = $(MEM_CODE_DIR)  
MEM_SRCC_DIR = $(MEM_CODE_DIR)
MEM_OBJ_DIR = $(MEM_CODE_DIR)


MEM_OBJS += $(MEM_OBJ_DIR)/mem_main.o

all: all-before  obj $(MEM_OBJS)  libMem.o all-after

obj:
	mkdir -p ${MEM_OBJ_DIR}

clean: clean-custom
	${RM} $(OBJ) $(BIN)  $(MEM_OBJS) $(CODE_DIR)/lib/libMem.o

$(MEM_OBJ_DIR)/mem_main.o: $(MEM_SRCC_DIR)/mem_main.c
	$(CC) -c  $(COMPILATION_SWITCH) $(MEM_SRCC_DIR)/mem_main.c -o $(MEM_OBJ_DIR)/mem_main.o $(CFLAGS) -I$(MEM_INC_DIR) -I$(CMN_INC_DIR)

libMem.o: $(MEM_OBJS)
	$(LD) $(LD_FLAGS) -o $(CODE_DIR)/lib/libMem.o $(MEM_OBJS)
