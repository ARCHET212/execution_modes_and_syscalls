#include "board.h"

void board_watchdog_disable(void) {
    // En QEMU (VersatilePB) no es necesario deshabilitar 
    // un watchdog de hardware para evitar que se reinicie el sistema.
    // Mantenemos la función para no romper las llamadas del kernel.
}