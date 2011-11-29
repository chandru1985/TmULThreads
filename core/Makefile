
#   $Id: Makefile,v 1.12 2011/01/23 10:36:23 Sasi Exp $ 
#!/bin/bash
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

CMN_INC_DIR = $(CODE_DIR)/cmn
CMN_SRC_DIR = $(CODE_DIR)/cmn

core_CODE_DIR = $(CODE_DIR)/core
core_INC_DIR = $(core_CODE_DIR)  
core_SRCC_DIR = $(core_CODE_DIR)
core_OBJ_DIR = $(core_CODE_DIR)/obj


ifeq ($(SCHED), SFS)
core_OBJS += $(core_OBJ_DIR)/sched.o
core_OBJS += $(core_OBJ_DIR)/setjmp.o
core_OBJS += $(core_OBJ_DIR)/thread_lib.o
core_OBJS += $(core_OBJ_DIR)/iostream.o
core_OBJS += $(core_OBJ_DIR)/soft_irq.o
core_OBJS += $(core_OBJ_DIR)/interrupt.o
core_OBJS += $(core_OBJ_DIR)/lock.o
core_OBJS += $(core_OBJ_DIR)/switch_to_os.o
endif
core_OBJS += $(core_OBJ_DIR)/rbtree.o
core_OBJS += $(core_OBJ_DIR)/hashlib.o
core_OBJS += $(core_OBJ_DIR)/tmlibinit.o
core_OBJS += $(core_OBJ_DIR)/msg_queue.o
ifneq ($(SCHED), SFS)
core_OBJS += $(core_OBJ_DIR)/common.o
endif

#For Code Coverage , The following switch are added
#COMPILATION_SWITCH += -fprofile-arcs -ftest-coverage

all: all-before  obj $(core_OBJS)  libCore.o all-after

obj:
	mkdir -p ${core_OBJ_DIR}

clean: clean-custom
	${RM} $(OBJ) $(BIN)  $(core_OBJS) $(CODE_DIR)/lib/libCore.o
	${RM} -r ${core_OBJ_DIR}



$(core_OBJ_DIR)/sched.o: $(core_SRCC_DIR)/sched.c
	$(CC) -c  $(COMPILATION_SWITCH) $(core_SRCC_DIR)/sched.c -o $(core_OBJ_DIR)/sched.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)

$(core_OBJ_DIR)/thread_lib.o: $(core_SRCC_DIR)/thread_lib.c
	$(CC) -c $(COMPILATION_SWITCH) $(core_SRCC_DIR)/thread_lib.c -o $(core_OBJ_DIR)/thread_lib.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)

$(core_OBJ_DIR)/iostream.o: $(core_SRCC_DIR)/iostream.c
	$(CC) -c $(COMPILATION_SWITCH) $(core_SRCC_DIR)/iostream.c -o $(core_OBJ_DIR)/iostream.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)

$(core_OBJ_DIR)/soft_irq.o: $(core_SRCC_DIR)/soft_irq.c
	$(CC) -c $(COMPILATION_SWITCH) $(core_SRCC_DIR)/soft_irq.c -o $(core_OBJ_DIR)/soft_irq.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)

$(core_OBJ_DIR)/interrupt.o: $(core_SRCC_DIR)/interrupt.c
	$(CC) -c $(COMPILATION_SWITCH) $(core_SRCC_DIR)/interrupt.c -o $(core_OBJ_DIR)/interrupt.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)


$(core_OBJ_DIR)/setjmp.o: $(core_SRCC_DIR)/setjmp.S
	$(CC) -c  $(COMPILATION_SWITCH) $(core_SRCC_DIR)/setjmp.S -o $(core_OBJ_DIR)/setjmp.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)

$(core_OBJ_DIR)/rbtree.o: $(CMN_SRC_DIR)/rbtree.c
	$(CC) -c  $(COMPILATION_SWITCH) $(CMN_SRC_DIR)/rbtree.c -o $(core_OBJ_DIR)/rbtree.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)

$(core_OBJ_DIR)/hashlib.o: $(CMN_SRC_DIR)/hashlib.c
	$(CC) -c  $(COMPILATION_SWITCH) $(CMN_SRC_DIR)/hashlib.c -o $(core_OBJ_DIR)/hashlib.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)


$(core_OBJ_DIR)/lock.o: $(core_SRCC_DIR)/lock.c
	$(CC) -c  $(COMPILATION_SWITCH) $(core_SRCC_DIR)/lock.c -o $(core_OBJ_DIR)/lock.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)

$(core_OBJ_DIR)/common.o: $(core_SRCC_DIR)/common.c
	$(CC) -c  $(COMPILATION_SWITCH) $(core_SRCC_DIR)/common.c -o $(core_OBJ_DIR)/common.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)

$(core_OBJ_DIR)/tmlibinit.o: $(core_SRCC_DIR)/tmlibinit.c
	$(CC) -c  $(COMPILATION_SWITCH) $(core_SRCC_DIR)/tmlibinit.c -o $(core_OBJ_DIR)/tmlibinit.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)

$(core_OBJ_DIR)/switch_to_os.o: $(core_SRCC_DIR)/switch_to_os.c
	$(CC) -c  $(COMPILATION_SWITCH) $(core_SRCC_DIR)/switch_to_os.c -o $(core_OBJ_DIR)/switch_to_os.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)

$(core_OBJ_DIR)/msg_queue.o: $(core_SRCC_DIR)/msg_queue.c
	$(CC) -c  $(COMPILATION_SWITCH) $(core_SRCC_DIR)/msg_queue.c -o $(core_OBJ_DIR)/msg_queue.o $(CFLAGS) -I$(core_INC_DIR) -I$(CMN_INC_DIR)


libCore.o: $(core_OBJS)
	$(LD) $(LD_FLAGS) -o $(CODE_DIR)/lib/libCore.o $(core_OBJS)
