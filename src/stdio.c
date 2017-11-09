#include "stdio.h"

void writel(uint32_t addr, uint32_t data) {
  *((uint32_t *)addr) = data;
}

uint32_t readl(uint32_t addr) {
  return *((uint32_t *)addr);
}
