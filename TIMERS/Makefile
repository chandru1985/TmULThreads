
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

TIMER_CODE_DIR = $(CODE_DIR)/TIMERS
TIMER_INC_DIR =  $(TIMER_CODE_DIR)/inc  -I$(CODE_DIR)/cmn 
TIMER_SRCC_DIR = $(TIMER_CODE_DIR)/src
TIMER_OBJ_DIR = $(TIMER_CODE_DIR)/obj

TIMER_OBJS += $(TIMER_OBJ_DIR)/btm_hlf.o
TIMER_OBJS += $(TIMER_OBJ_DIR)/tmr_init.o
TIMER_OBJS += $(TIMER_OBJ_DIR)/update.o
TIMER_OBJS += $(TIMER_OBJ_DIR)/thr_spec.o

all: all-before  obj $(TIMER_OBJS)  libTmTimers.o all-after

obj:
	mkdir -p ${TIMER_OBJ_DIR}

clean: clean-custom
	${RM} $(OBJ) $(BIN)  $(TIMER_OBJ_DIR)/*.o $(CODE_DIR)/lib/libTmTimers.o
	${RM} -r ${TIMER_OBJ_DIR}

$(TIMER_OBJ_DIR)/tmr_init.o: $(TIMER_SRCC_DIR)/tmr_init.c
	$(CC) -c $(COMPILATION_SWITCH) $(TIMER_SRCC_DIR)/tmr_init.c -o $(TIMER_OBJ_DIR)/tmr_init.o $(CFLAGS) -I$(TIMER_INC_DIR)

$(TIMER_OBJ_DIR)/btm_hlf.o: $(TIMER_SRCC_DIR)/btm_hlf.c
	$(CC) -c $(COMPILATION_SWITCH) $(TIMER_SRCC_DIR)/btm_hlf.c -o $(TIMER_OBJ_DIR)/btm_hlf.o $(CFLAGS) -I$(TIMER_INC_DIR)

$(TIMER_OBJ_DIR)/update.o: $(TIMER_SRCC_DIR)/update.c
	$(CC) -c $(COMPILATION_SWITCH) $(TIMER_SRCC_DIR)/update.c -o $(TIMER_OBJ_DIR)/update.o $(CFLAGS) -I$(TIMER_INC_DIR)

$(TIMER_OBJ_DIR)/thr_spec.o: $(TIMER_SRCC_DIR)/thr_spec.c
	$(CC) -c $(COMPILATION_SWITCH) $(TIMER_SRCC_DIR)/thr_spec.c -o $(TIMER_OBJ_DIR)/thr_spec.o $(CFLAGS) -I$(TIMER_INC_DIR)


libTmTimers.o: $(TIMER_OBJS)
	$(LD) $(LD_FLAGS) -o $(CODE_DIR)/lib/libTmTimers.o $(TIMER_OBJS)
#lock.o: $(TIMER_SRCC_DIR)/lock.c
#	$(CC) -c $(COMPILATION_SWITCH) $(TIMER_SRCC_DIR)/lock.c -o lock.o $(CFLAGS) -I$(TIMER_INC_DIR)
