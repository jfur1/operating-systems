#include<stdio.h>
#include<linux/kernel.h>
#include<sys/syscall.h>
#include<unistd.h>

int main()
{
	long int amma = syscall(548);
	printf("System call returned %ld\n", amma);
	return 0;
}
