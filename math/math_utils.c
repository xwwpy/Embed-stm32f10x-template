#include "math_utils.h"

/**
 * @brief 实现整数的幂
 * 
 * @param source 底数
 * @param exp 幂次
 * @return uint32_t 
 */
u32 pow(u8 source, u8 exp) {

    u32 power = 1;
    u8 i = 1;
    for(; i <= exp; i++) {
        power *= source;
    }
    return power;

}

u32 abs(s32 val) {
    if(val < 0) {
        return (u32) -val;
    }
    return (u32) val;
}
