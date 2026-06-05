# --- Selección de Placa (Por defecto BBB) ---
BOARD ?= BBB

# --- Configuración de Rutas ---
BOARD_DIR = boards/$(BOARD)
OS_DIR    = os
LIB_DIR   = lib
USER_DIR  = user
USER_LIB_DIR = $(USER_DIR)/lib
P1_DIR    = $(USER_DIR)/p1
P2_DIR    = $(USER_DIR)/p2

# --- Carpeta de Salida ---
BUILD_DIR = build_$(BOARD)

# --- Configuración por Arquitectura ---
ifeq ($(BOARD), qemu)
    CPU       = arm926ej-s
    P1_ADDR   = 0x00110000
    P2_ADDR   = 0x00210000
    BOARD_DEF = -D_QEMU_
else
    CPU       = cortex-a8
    P1_ADDR   = 0x82100000
    P2_ADDR   = 0x82200000
    BOARD_DEF = -D_BBB_
endif

# --- Herramientas ---
CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-as
LD      = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

# --- Flags ---
# SE AGREGARON LAS RUTAS DE LIBRERÍAS AL INCLUDE (-I$(LIB_DIR) -I$(USER_LIB_DIR))
CFLAGS  = -mcpu=$(CPU) $(BOARD_DEF) -Wall -ffreestanding -nostdlib -I$(OS_DIR) -I$(BOARD_DIR) -I$(LIB_DIR) -I$(USER_LIB_DIR) -g
ASFLAGS = -mcpu=$(CPU) -g

# --- Listado de Objetos ---
# SE AÑADIERON LOS NUEVOS MÓDULOS AL KERNEL: syscalls.o, faults.o, stdio.o, string.o
KERNEL_OBJS = $(BUILD_DIR)/root.o $(BUILD_DIR)/os.o $(BUILD_DIR)/pcb.o \
              $(BUILD_DIR)/timer.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/mem.o \
              $(BUILD_DIR)/uart_hw.o $(BUILD_DIR)/timer_hw.o $(BUILD_DIR)/wdt_hw.o \
              $(BUILD_DIR)/syscalls.o $(BUILD_DIR)/faults.o \
              $(BUILD_DIR)/stdio.o $(BUILD_DIR)/string.o

# LOS PROCESOS DE USUARIO YA NO COMPILAN UART_HW. AHORA USAN SU LIBRERÍA DE SYSCALLS
P1_OBJS     = $(BUILD_DIR)/p1_user_root.o $(BUILD_DIR)/p1_main.o \
              $(BUILD_DIR)/p1_syscall_api.o $(BUILD_DIR)/mem.o

P2_OBJS     = $(BUILD_DIR)/p2_user_root.o $(BUILD_DIR)/p2_main.o \
              $(BUILD_DIR)/p2_syscall_api.o $(BUILD_DIR)/mem.o

# --- Reglas Principales ---
all: setup $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/p1.bin $(BUILD_DIR)/p2.bin

# Crear carpeta de build si no existe
setup:
	@mkdir -p $(BUILD_DIR)

# --- Compilación del Kernel ---
$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/kernel.elf: $(KERNEL_OBJS)
	$(LD) -T $(BOARD_DIR)/linker.ld $^ -o $@

# --- Compilación de Procesos con Inyección de Dirección ---
$(BUILD_DIR)/p1.bin: $(P1_OBJS)
	$(LD) -T $(P1_DIR)/linker_p1.ld --defsym=PROG_START=$(P1_ADDR) $^ -o $(BUILD_DIR)/p1.elf
	$(OBJCOPY) -O binary $(BUILD_DIR)/p1.elf $@

$(BUILD_DIR)/p2.bin: $(P2_OBJS)
	$(LD) -T $(P2_DIR)/linker_p2.ld --defsym=PROG_START=$(P2_ADDR) $^ -o $(BUILD_DIR)/p2.elf
	$(OBJCOPY) -O binary $(BUILD_DIR)/p2.elf $@

# --- Reglas genéricas del Kernel y librerías ---
$(BUILD_DIR)/%.o: $(OS_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(BOARD_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# NUEVA REGLA PARA COMPILAR LIBRERÍAS (stdio.c, string.c)
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/root.o: $(BOARD_DIR)/root.s
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/mem.o: $(OS_DIR)/mem.s
	$(AS) $(ASFLAGS) $< -o $@

# --- Objetos específicos de procesos ---
$(BUILD_DIR)/p1_main.o: $(P1_DIR)/p1_main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/p1_user_root.o: $(P1_DIR)/user_root.s
	$(AS) $(ASFLAGS) $< -o $@

# NUEVA REGLA PARA LA API DEL P1
$(BUILD_DIR)/p1_syscall_api.o: $(USER_LIB_DIR)/syscall_api.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/p2_main.o: $(P2_DIR)/p2_main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/p2_user_root.o: $(P2_DIR)/user_root.s
	$(AS) $(ASFLAGS) $< -o $@

# NUEVA REGLA PARA LA API DEL P2
$(BUILD_DIR)/p2_syscall_api.o: $(USER_LIB_DIR)/syscall_api.c
	$(CC) $(CFLAGS) -c $< -o $@


# --- Utilidades ---
clean:
	rm -rf build_BBB build_qemu

run:
	qemu-system-arm -M versatilepb -m 128M -nographic \
		-serial mon:stdio \
		-kernel $(BUILD_DIR)/kernel.elf \
		-device loader,file=$(BUILD_DIR)/p1.bin,addr=$(P1_ADDR) \
		-device loader,file=$(BUILD_DIR)/p2.bin,addr=$(P2_ADDR)

debug:
	qemu-system-arm -M versatilepb -m 128M -nographic \
        -serial mon:stdio \
        -kernel $(BUILD_DIR)/kernel.elf \
        -device loader,file=$(BUILD_DIR)/p1.bin,addr=$(P1_ADDR) \
        -device loader,file=$(BUILD_DIR)/p2.bin,addr=$(P2_ADDR) \
        -s -S