# shared-memory-example

This is a dummy program that demonstrates how to achieve interprocess communication using the shared memory method: allowing two processes to read/write to the same locations in memory.

First, we call `shm_open()` to create a shared memory object. See the code for relevant details about the arguments we provide to it. But basically, we create a descriptor, similar to a file descriptor, that allows different processes to read/write to the same memory region using `mmap()`.

Then, we actually make a call to `mmap()` to allocate this shared memory. This creates a region of memory (of size that we can specify) that can be shared between processes. We then write to this area of memory, and use fork() to create another (child) process that reports what it read from it.

Finally, we clean up the shared memory by calling `munmap()` and `shm_unlink()`.
