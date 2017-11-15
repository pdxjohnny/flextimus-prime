#ifndef _STDIO_H
#define _STDIO_H

#include <stdint.h>

void writel(uint32_t addr, uint32_t data);
uint32_t readl(uint32_t addr);

#endif /* _STDIO_H */
