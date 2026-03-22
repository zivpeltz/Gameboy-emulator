#include "registers.h"



/* number returned by every function is the amount of CPU cycles the simulated run of that function takes */

/* register to register 8-bit LD function*/
int load_R2R(RegisterIndex src, int src_high, RegisterIndex dst, int dst_high) {
    set8(dst, dst_high, get8(src,src_high));
    return 4; 
}