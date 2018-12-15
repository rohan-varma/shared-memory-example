#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define STORAGE_ID "/SHM_TEST"
#define STORAGE_SIZE 32
#define DATA "Hello, World! From PID %d"

int main(int argc, char *argv[])
{
	int res;
	int fd;
	int len;
	pid_t pid;
	void *addr;
	char data[STORAGE_SIZE];

	pid = getpid();
	sprintf(data, DATA, pid);

	// try shm_unlink if it didnt work on the last run of this
	fd = shm_unlink(STORAGE_ID);
	if (fd == -1)
	{
		; //eh, whatever...?
	}

	// get shared memory file descriptor (NOT a file)
	fd = shm_open(STORAGE_ID, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		perror("open");
		return 10;
	}
	fprintf(stderr, "made it here\n");

	// extend shared memory object as by default it's initialized with size 0
	res = ftruncate(fd, STORAGE_SIZE);
	if (res == -1)
	{
		perror("ftruncate");
		return 20;
	}

	// map shared memory to process address space
	addr = mmap(
		NULL,
		STORAGE_SIZE, 
		PROT_WRITE | PROT_READ, 
		MAP_SHARED /* shared memory flag*/,
		 fd,
		 0
		);
	if (addr == MAP_FAILED)
	{
		perror("mmap");
		return 30;
	}

	// place data into memory
	len = strlen(data) + 1;
	memcpy(addr, data, len);

	fprintf(stderr, "sleeping\n");
	// wait for someone to read it
	sleep(2);
	fprintf(stderr, "awake\n");

	// mmap cleanup
	res = munmap(addr, STORAGE_SIZE);
	if (res == -1)
	{
		perror("munmap");
		return 40;
	}

	// shm_open cleanup
	fd = shm_unlink(STORAGE_ID);
	if (fd == -1)
	{
		perror("unlink");
		return 100;
	}

	return 0;
}