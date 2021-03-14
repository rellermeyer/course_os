#include <dtb.h>

// DTB is in big endian, so it must be converted to little endian
static inline uint32_t fix_endian(uint32_t num){
  char *ptr = (char*)&num;
  int ret;
  
  ret = ptr[0] << 24;
  ret |= ptr[1] << 16;
  ret |= ptr[2] << 8;
  ret |= ptr[3];

  return ret;
}

void dtb_get_property(struct DTHeader* dtb_h, char* path, char* property){
  if(fix_endian(dtb_h->magic)!=0xd00dfeed){
    FATAL("Wrong dtb header %x", dtb_h->magic);
  }
}


