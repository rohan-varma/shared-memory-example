#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <memory.h>
#include <stdlib.h>


char const * storage_filepath = "shm_test";
int file_length = 512; //bytes

int main(int argc, char* argv[]) {
	shm_unlink(storage_filepath); // try to unlink shared memory from before if we failed and didnt unlink
	pid_t process_pid = getpid();
	int memory_fd = shm_open(
		"shm_test",
		O_RDWR | O_CREAT, /* open for reading, and create if not exists */
		S_IRUSR | S_IWUSR /* set permissions: read for owner, write for owner */
	);
	if (memory_fd == -1) {
		fprintf(stderr, "Error with shm_open\n");
		fprintf(stderr, "%s\n", strerror(errno));
	}
	// extend memory size by file_length bytes
	int extend = ftruncate(memory_fd, file_length);
	if (extend == -1) {
		fprintf(stderr, "ftruncate() failed\n");
	}
	void* memory_address = mmap(
		NULL, /* let system slect the address to map */
		file_length, /* map file_length bytes */
		PROT_WRITE, /* write and read permissions to the memory */
		MAP_SHARED, /* needed for shared memory so that multiple processes can access */
		memory_fd, /* memory file descriptor */
		0 /* offset */
	);
	if (memory_address == MAP_FAILED) {
		fprintf(stderr, "map failed\n");
	}
	// write data to shared memory
	char* data = (char *) malloc(100 * sizeof(char));
	fprintf(stderr, "Writing hello world in main process\n");
	sprintf(data, "What's up? Peeps. From process %d\n", process_pid);
	memcpy(memory_address, data, strlen(data));
	// create a separate process
	int fork_ret = fork();
	if (fork_ret == -1) {
		fprintf(stderr, "fork() failed\n");
	}
	if (fork_ret == 0) {
		// in child process
		// do a read
		char * read_data = (char *) malloc(100 * sizeof(char));
		memcpy(read_data, memory_address, strlen(data));
		fprintf(stderr, "in child, read:%s\n", read_data);
	} else {
		// wait for child process to read
		pid_t child_pid = wait(0);
		// clean up, unmap the mmaped addresses and unlink shared memory.
		munmap(memory_address, file_length);
		shm_unlink(storage_filepath);
	}
	return 0;
}