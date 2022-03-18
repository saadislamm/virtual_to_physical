#include <stdio.h>
#include <stdint.h>		// For uint64_t
#include <fcntl.h>		// For O_RDONLY in get_physical_addr fn
#include <unistd.h>		// For pread in get_physical_addr fn

// get_physical_addr function has been taken from
// https://github.com/IAIK/flipfloyd/blob/master/waylaying/check.c

static uint64_t get_physical_addr(uint64_t virtual_addr) {
  static int g_pagemap_fd = -1;
  uint64_t value;
  
  // open the pagemap
  if(g_pagemap_fd == -1) {
      g_pagemap_fd = open("/proc/self/pagemap", O_RDONLY);
  }
  if(g_pagemap_fd == -1) return 0;
  
  // read physical address
  off_t offset = (virtual_addr / 4096) * sizeof(value);
  int got = pread(g_pagemap_fd, &value, sizeof(value), offset);
  if(got != 8) return 0;

  // Check the "page present" flag.
  if(!(value & (1ULL << 63))) return 0;

  // return physical address
  uint64_t frame_num = value & ((1ULL << 55) - 1);
  return (frame_num * 4096) | (virtual_addr & (4095));
}

void main() {

	int a = 0;
	int b = 0;
	
	printf("\nVirtual address of a: %p \n", &a);
	printf("Virtual address of b: %p \n\n", &b);
	
	printf("Physical address of a: %lx \n", get_physical_addr((uint64_t)&a));
	printf("Physical address of b: %lx \n\n", get_physical_addr((uint64_t)&b));
}
