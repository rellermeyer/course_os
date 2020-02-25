#ifndef __VM_FRAME_H
#define __VM_FRAME_H

int vm_build_free_frame_list(void *start, void *end);
void *vm_get_free_frame();
void vm_release_frame(void *p);
int vm_count_free_frames();

#endif
