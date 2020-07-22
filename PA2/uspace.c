#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<malloc.h>
#include<string.h>

#define BUFFER_SIZE 1024

int main(){
	char option, buffer[BUFFER_SIZE];
	int u_offset, u_whence, length;

	int file = open("/dev/pa2_char_driver", O_RDWR);
	
	printf("Userspace Test:\n");
	
	while(1){
		printf("\nOptions:\nPress r to read from device\nPress w to write to device\nPress s to seek into device\nPress e to exit from the device\nPress anything else to keep reading or writing from the device\nEnter command:\n");
	
		scanf("%c", &option);
		switch(option){
			case 'r':{
				//printf("Enter the number of bytes you want to read:\n");
				//scanf("%d", &length);
				//char* buffer = (char*)malloc(sizeof(char) *length);
				read(file, buffer, BUFFER_SIZE);
				printf("Device read output: %s\n", buffer);
				//free(buffer);
				while(getchar() != '\n');
				break;}
			case 'w':{
				//char* data = (char*)malloc(1024 * sizeof(char));
				printf("Enter the data you want to write to the device:\n");
				scanf(" %[^\n]",buffer);
				//length = strlen(data);
				write(file, buffer, BUFFER_SIZE);
				//free(data);
				while(getchar() != '\n');
				break;}
			case 's':{
				printf("Enter an offset value:\n");
				scanf("%d", &u_offset);
				printf("Enter a value for whence: \n");
				scanf("%d", &u_whence);
				lseek(file, u_offset, u_whence);
				while(getchar() != '\n');
				break;}
			case 'e':
				printf("Exiting the test app.\n");
				close(file);
				return 0;
			default:
				while(getchar != '\n');
		}

	}
	close(file);
	return 0;

}
