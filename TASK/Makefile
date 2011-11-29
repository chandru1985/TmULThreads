
#   $Id: Makefile,v 1.4 2010/10/17 17:05:26 Sasi Exp $ 
#!/bin/bash
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h


CMN_INC_DIR = $(CODE_DIR)/DS/SLL/inc

TASK_CODE_DIR = $(CODE_DIR)/TASK
TASK_INC_DIR = $(TASK_CODE_DIR)/inc -I$(CODE_DIR)/cmn
TASK_SRCC_DIR = $(TASK_CODE_DIR)/src
TASK_OBJ_DIR = $(TASK_CODE_DIR)/obj


TASK_OBJS += $(TASK_OBJ_DIR)/tsk_util.o
TASK_OBJS += $(TASK_OBJ_DIR)/tsk_main.o

ifeq ($(SCHED), SFS)
TASK_OBJS += $(TASK_OBJ_DIR)/tsk_sched.o
else
    ifeq ($(TARGET_OS), LINUX)
        TASK_OBJS += $(TASK_OBJ_DIR)/tsk_lnx.o
    else
      ifeq ($(TARGET_OS), WINDOWS)
          TASK_OBJS += $(TASK_OBJ_DIR)/tsk_win.o
      else
          TASK_OBJS += $(TASK_OBJ_DIR)/tsk_stub.o
      endif
    endif
endif

#For Code Coverage , The following switch are added
#COMPILATION_SWITCH += -fprofile-arcs -ftest-coverage

all: all-before  obj $(TASK_OBJS)  libTmTask.o all-after

obj:
	mkdir -p ${TASK_OBJ_DIR}

clean: clean-custom
	${RM} $(OBJ) $(BIN)  $(TASK_OBJS) $(CODE_DIR)/lib/libTmTask.o
	${RM} -r ${TASK_OBJ_DIR}

$(TASK_OBJ_DIR)/tsk_main.o: $(TASK_SRCC_DIR)/tsk_main.c
	$(CC) -c $(COMPILATION_SWITCH) $(TASK_SRCC_DIR)/tsk_main.c -o $(TASK_OBJ_DIR)/tsk_main.o $(CFLAGS) -I$(TASK_INC_DIR) -I$(CMN_INC_DIR)

$(TASK_OBJ_DIR)/tsk_lnx.o: $(TASK_SRCC_DIR)/tsk_lnx.c
	$(CC) -c $(COMPILATION_SWITCH) $(TASK_SRCC_DIR)/tsk_lnx.c -o $(TASK_OBJ_DIR)/tsk_lnx.o $(CFLAGS) -I$(TASK_INC_DIR) -I$(CMN_INC_DIR)

$(TASK_OBJ_DIR)/tsk_util.o: $(TASK_SRCC_DIR)/tsk_util.c
	$(CC) -c $(COMPILATION_SWITCH) $(TASK_SRCC_DIR)/tsk_util.c -o $(TASK_OBJ_DIR)/tsk_util.o $(CFLAGS) -I$(TASK_INC_DIR) -I$(CMN_INC_DIR)

$(TASK_OBJ_DIR)/tsk_win.o: $(TASK_SRCC_DIR)/tsk_win.c
	$(CC) -c $(COMPILATION_SWITCH) $(TASK_SRCC_DIR)/tsk_win.c -o $(TASK_OBJ_DIR)/tsk_win.o $(CFLAGS) -I$(TASK_INC_DIR) -I$(CMN_INC_DIR)

$(TASK_OBJ_DIR)/tsk_stub.o: $(TASK_SRCC_DIR)/tsk_stub.c
	$(CC) -c $(COMPILATION_SWITCH) $(TASK_SRCC_DIR)/tsk_stub.c -o $(TASK_OBJ_DIR)/tsk_stub.o $(CFLAGS) -I$(TASK_INC_DIR) -I$(CMN_INC_DIR)

$(TASK_OBJ_DIR)/tsk_sched.o: $(TASK_SRCC_DIR)/tsk_sched.c
	$(CC) -c $(COMPILATION_SWITCH) $(TASK_SRCC_DIR)/tsk_sched.c -o $(TASK_OBJ_DIR)/tsk_sched.o $(CFLAGS) -I$(TASK_INC_DIR) -I$(CMN_INC_DIR)

libTmTask.o: $(TASK_OBJS)
	$(LD) $(LD_FLAGS) -o $(CODE_DIR)/lib/libTmTask.o $(TASK_OBJS)

