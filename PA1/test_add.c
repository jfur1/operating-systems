#include<sys/syscall.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main()
{
	int num1 = 5;
	int num2 = 3;

	// Allocate an integer on the stack
	int c;
	
	// Perform system call, passing address we want to store result in
	syscall(333, num1, num2, &c);

	// Print out results with the dereferenced ptr
	 printf("Value of %d + %d = %d\n", num1, num2, c);

	return 0;
}
