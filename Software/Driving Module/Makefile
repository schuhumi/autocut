MPY_PATH = ./micropython
BUILD_PATH = ./build
SRC_PATH = ./src

PORT = stm32
BOARD = NUCLEO_F401RE
ARCH = armv7emsp
DEVICE = /dev/ttyACM0
BAUD = 115200

src_files = $(wildcard $(SRC_PATH)/*.py)
mpy_files = $(patsubst $(SRC_PATH)/%.py,$(BUILD_PATH)/%.mpy,$(src_files))

PYBOARD = $(MPY_PATH)/tools/pyboard.py
MPY_CROSS = $(MPY_PATH)/mpy-cross/mpy-cross
MPY_CROSS_ALL = $(MPY_PATH)/tools/mpy_cross_all.py

erase_cmd  = "import os; [os.remove(each) for each in os.listdir()]; import machine; machine.reset()"
boot_cmd   = "from main import *"
reboot_cmd = "import machine; machine.reset()"

default: clean install

proper: mpy_clean mpy_install clean install

test:
	@echo src_files: $(src_files)
	@echo mpy_files: $(mpy_files)

mpy_clean:
	$(MAKE) -C $(MPY_PATH)/mpy-cross clean
	$(MAKE) -C $(MPY_PATH)/ports/$(PORT) clean

mpy_all:
	$(MAKE) -C $(MPY_PATH)/mpy-cross
	$(MAKE) -C $(MPY_PATH)/ports/$(PORT) BOARD=$(BOARD) submodules
	$(MAKE) -C $(MPY_PATH)/ports/$(PORT) BOARD=$(BOARD)
	
mpy_install: mpy_all
	$(MAKE) -C $(MPY_PATH)/ports/$(PORT) BOARD=$(BOARD) deploy-stlink
	
clean:
	@echo --- cleaning up ---
	rm -rf $(BUILD_PATH)

$(BUILD_PATH)/%.mpy: $(SRC_PATH)/%.py
	mkdir -p $(BUILD_PATH)
	$(MPY_CROSS) -o $@ -march=$(ARCH) $<

install: $(mpy_files)
	@echo --- installing ---
	echo $(erase_cmd) > $(BUILD_PATH)/_erase.py
	$(PYBOARD) --device=$(DEVICE) -b $(BAUD) $(BUILD_PATH)/_erase.py --no-follow
	sleep 2 # Wait for reboot after erase...
	echo $(boot_cmd) > $(BUILD_PATH)/boot.py
	$(PYBOARD) --device=$(DEVICE) -b $(BAUD) -f cp $(BUILD_PATH)/boot.py $(BUILD_PATH)/_erase.py $(mpy_files) :
	@echo installed files:
	$(PYBOARD) --device=$(DEVICE) -b $(BAUD) -f ls

run: $(mpy_files)
	@echo --- resetting to run from beginning ---
	$(PYBOARD) --device=$(DEVICE) -b $(BAUD) -c $(reboot_cmd)
	
interactive:
	@echo --- interactive shell \(press Ctrl+A and Ctrl+Q to quit\) ---
	picocom $(DEVICE) -b$(BAUD)
	
	
.PHONY: default proper test mpy_clean mpy_all mpy_install install run interactive
