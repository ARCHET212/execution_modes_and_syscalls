#include "board.h"

void board_watchdog_disable(void) {
    // Disable watchdog timer (WDT1) on AM335x
    // The watchdog will reset the board if not disabled or serviced periodically
    // Sequence: Write 0xAAAA to WDT_WSPR, wait for WWPS, then write 0x5555
    
    // First disable sequence
    PUT32(WDT_WSPR, 0xAAAA);
    // Wait for write posting to complete
    while (GET32(WDT_WWPS) != 0);
    
    // Second disable sequence
    PUT32(WDT_WSPR, 0x5555);
    // Wait for write posting to complete
    while (GET32(WDT_WWPS) != 0);
    
    // Verify watchdog is disabled (optional, but good for debugging)
    // WDT_WCLR should be 0 when disabled
}