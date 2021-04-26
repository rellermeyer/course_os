# Allocator
> :warning: This allocator puts memory next to each other, _NEVER_
> rely on the memory next to yours being NULL. There is a pretty
> decent change it will not only be allocated, but by either an
> old variable or a similar structure and cause an infinite loop.

## Description
A buddy allocator implemented consisting out of two parts:
- [Implementation](include/allocator.h)
- [Interface](include/mem_alloc.h)

## Notes
There are a few issues with the current implementation of the memory
allocator, one of which is the fact that it is badly documented. It is
a bit hard to figure out what terms as "order" in this context means
and in which order it goes. Another important issue is the fact that
most of the code seems to be directly copied from the following [Github
repository](https://github.com/block8437/buddyalloc), which is
unlicensed and hence is propietary. For those of these reasons this
allocator is a prime target for a rewrite in a future iteration of the
course.
