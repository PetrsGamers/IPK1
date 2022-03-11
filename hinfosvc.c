#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
// Function that will load to the buffer name of the computer
void gethostnames(char *buffer)
{
	FILE *fptr;
	char *path[255];
	*path = "cat /proc/sys/kernel/hostname"; 
	fptr = popen(*path, "r"); // this function will open a shell command and load it into file
	fgets(buffer, 256, fptr);
	fclose(fptr);
	return;
}
// Function that will load to the buffer name of the CPU
void getcpuname(char *buffer)
{
	FILE *fptr;
	char *path[255];
	*path = "cat /proc/cpuinfo | grep 'model name' | head -n 1 | cut -d':' -f2- | cut -b 2-"; // this function will open a shell command and load it into file
	fptr = popen(*path, "r");
	fgets(buffer, 256, fptr);
	fclose(fptr);
	return;
}
// Function that will load to the CPU_percentage actual usage of the processor
int getcpuload()
{
	FILE *fptr;
	char *path[255];
	char buffer2[256];
	char buffer[256];
	*path = "cat /proc/stat | head -n 1"; 
	fptr = popen(*path, "r");
	fgets(buffer, 256, fptr);
	sleep(1); // we need to have to cpu stats to calculate cpu load
	fptr = popen(*path, "r");
	fgets(buffer2, 256, fptr); // to have a precise number we have to calculate 
	int i = 0;
	char *p = strtok(buffer, " ");
	char *array[11];
	while (p != NULL) //this will create a string array of the information that we need to get form /proc/stat
	{
		array[i++] = p;
		p = strtok(NULL, " ");
	}
	int prevuser = atoi(array[1]); // this will create a variables needed for the calculation
	int prevnice = atoi(array[2]);
	int prevsystem = atoi(array[3]);
	int previdle = atoi(array[4]);
	int previowait = atoi(array[5]);
	int previrq = atoi(array[6]);
	int prevsoftirq = atoi(array[7]);
	int prevsteal = atoi(array[8]);

	int j = 0;
	char *k = strtok(buffer2, " ");
	char *array2[11]; 

	while (k != NULL)
	{
		array2[j++] = k;
		k = strtok(NULL, " ");
	}

	int user = atoi(array2[1]);  // this will create a variables needed for the calculation
	int nice = atoi(array2[2]);
	int system = atoi(array2[3]);
	int idle = atoi(array2[4]);
	int iowait = atoi(array2[5]);
	int irq = atoi(array2[6]);
	int softirq = atoi(array2[7]);
	int steal = atoi(array2[8]);
	int PrevIdle = previdle + previowait; // calculatio of the cpu load
	int Idle = idle + iowait;
	int PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
	int NonIdle = user + nice + system + irq + softirq + steal;
	int PrevTotal = PrevIdle + PrevNonIdle;
	int Total = Idle + NonIdle;
	double totald = Total - PrevTotal;
	double idled = Idle - PrevIdle;
	double CPU_Percentage = (totald - idled) / totald;
	CPU_Percentage = CPU_Percentage * 100; //create a %
	fclose(fptr);
	return CPU_Percentage;
}
int main(int argc, char const *argv[])
{
	if (argc == 1 || argc > 2) // check if there is a argument
	{
		return 1;
	}
	int intarg = atoi(argv[1]); 
	if (intarg > 65535 || intarg < 0) //check if the port has a value
	{
		exit(EXIT_FAILURE);
		return 1;
	}

	int server_fd;
	int new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) //create a socket
	{

		exit(EXIT_FAILURE);
		return 1;
	}

	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY;
	int port = atoi(argv[1]); 
	address.sin_port = htons(port); //set up for binding a server
	setsockopt(server_fd, 1, SO_REUSEPORT,&(int){1}, sizeof(int)); //setting to reuse ports
	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) //check if the binding was succesfull
	{

		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 10) < 0) //check id the listen is succesfull
	{

		exit(EXIT_FAILURE);
	}
	while (1)
	{

		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) //waiting for a request
		{
			exit(EXIT_FAILURE);
		}

		char buffer[30000] = {0};
		long valread = read(new_socket, buffer, 30000);

		if (strstr(buffer, "GET /hostname ")) //request is hostname
		{
			char buffer2[256];

			char string[1256] = "HTTP/1.1 200 OK\nContent-Type: text/plain;\r\n\r\n"; //create a http header
			gethostnames(buffer2);
			strcat(string, buffer2); // add to header hostname

			write(new_socket, string, strlen(string)); //return to a client
		}
		else if (strstr(buffer, "GET /cpu-name ")) //request is cpu-name
		{

			char buffer2[256];
			char string[1256] = "HTTP/1.1 200 OK\nContent-Type: text/plain;\r\n\r\n"; //create a http header
			getcpuname(buffer2);
			strcat(string, buffer2); // add to header cpu-name
			write(new_socket, string, strlen(string));  //return to a client
		} 
		else if (strstr(buffer, "GET /load ")) //request is cpu-load
		{

			int cpu = getcpuload();
			char string[1256] = "HTTP/1.1 200 OK\nContent-Type: text/plain;\r\n\r\n"; //create a http header
			char str[3];
			sprintf(str, "%d", cpu); // add to header cpu-name
			strcat(string, str);
			strcat(string, "%\n"); // add to header %
			write(new_socket, string, strlen(string)); //return to a client
		}
		else
		{

			char string[1256] = "HTTP/1.1 400 Bad Request\r\n"; // if there was no allowed request create a Bad request header
			write(new_socket, string, strlen(string));
		}

		close(new_socket);
	}
	return 0;
}
