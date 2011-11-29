
include make.h

.PHONY: all all-before all-after clean clean-custom

all: all-before  lib TASK_LIB TIMERS_LIB CORE MEM libTm.o all-after

lib:
	mkdir -p ${LIB_DIR}

TIMERS_LIB:
	make -C $(CODE_DIR)/TIMERS
TASK_LIB:
	make -C $(CODE_DIR)/TASK
CORE:
	make -C $(CODE_DIR)/core
MEM:
	make -C $(CODE_DIR)/mem


libTm.o: TIMERS_LIB 
	@echo "libTm.o: Creating the Lib .........";\
	$(LD) $(LD_FLAGS) -o ../libTm-$(VERSION).o $(LIB_DIR)/*.o
	@echo "libTm.o: Lib creation done.....";

clean: clean-custom
	make clean -C $(CODE_DIR)/TIMERS
	make clean -C $(CODE_DIR)/TASK
	make clean -C $(CODE_DIR)/core
	make clean -C $(CODE_DIR)/mem
	rm -f $(LIB_DIR)/*.o $(CODE_DIR)/libTm-$(VERSION).o
	rm -rf $(LIB_DIR)
