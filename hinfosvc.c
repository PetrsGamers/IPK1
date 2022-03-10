#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
void gethostname(char *buffer)
{
	FILE *fptr;
	char *path[255];
	*path = "cat /proc/sys/kernel/hostname";
	fptr = popen(*path, "r");
	fgets(buffer, 256, fptr);
	fclose(fptr);
	return;
}
void getcpuname(char *buffer)
{
	FILE *fptr;
	char *path[255];
	*path = "cat /proc/cpuinfo | grep 'model name' | head -n 1 | cut -d':' -f2- | cut -b 2-";
	fptr = popen(*path, "r");
	fgets(buffer, 256, fptr);
	fclose(fptr);
	return;
}
int getcpuload()
{
	FILE *fptr;
	char *path[255];
	char *buffer2[256];
	char *buffer[256];
	*path = "cat /proc/stat | head -n 1";
	fptr = popen(*path, "r");
	fgets(buffer, 256, fptr);
	sleep(1);
	fptr = popen(*path, "r");
	fgets(buffer2, 256, fptr);
	int i = 0;
	char *p = strtok(buffer, " ");
	char *array[11];
	while (p != NULL)
	{
		array[i++] = p;
		p = strtok(NULL, " ");
	}
	int prevuser = atoi(array[1]);
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

	int user = atoi(array2[1]);
	int nice = atoi(array2[2]);
	int system = atoi(array2[3]);
	int idle = atoi(array2[4]);
	int iowait = atoi(array2[5]);
	int irq = atoi(array2[6]);
	int softirq = atoi(array2[7]);
	int steal = atoi(array2[8]);
	int PrevIdle = previdle + previowait;
	int Idle = idle + iowait;
	int PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
	int NonIdle = user + nice + system + irq + softirq + steal;
	int PrevTotal = PrevIdle + PrevNonIdle;
	int Total = Idle + NonIdle;
	double totald = Total - PrevTotal;
	double idled = Idle - PrevIdle;
	double CPU_Percentage = (totald - idled) / totald;
	CPU_Percentage = CPU_Percentage * 100;
	fclose(fptr);
	return CPU_Percentage;
}
int main(int argc, char const *argv[])
{
	if (argc == 1 || argc > 2)
	{
		return 1;
	}
	int intarg = atoi(argv[1]);
	if (intarg > 65535 || intarg < 0)
	{
		exit(EXIT_FAILURE);
		return 1;
	}

	int server_fd, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
	{
		
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	int port = atoi(argv[1]);
	address.sin_port = htons(port);
	setsockopt(server_fd, 1, SO_REUSEADDR, SO_REUSEPORT, sizeof(int));
	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{

		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 10) < 0)
	{
		
		exit(EXIT_FAILURE);
	}
	while (1)
	{
		
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
		{
			exit(EXIT_FAILURE);
		}

		char buffer[30000] = {0};
		long valread = read(new_socket, buffer, 30000);
		
		if (strstr(buffer, "GET /hostname"))
		{
		char buffer2[256];
			
			char string[1256] = "HTTP/1.1 200 OK\nContent-Type: text/plain;\r\n\r\n";
			gethostname(buffer2);
			strcat(string, buffer2);

			write(new_socket, string, strlen(string));
		}
		else if (strstr(buffer, "GET /cpu-name"))
		{

			
		char buffer2[256];
			char string[1256] = "HTTP/1.1 200 OK\nContent-Type: text/plain;\r\n\r\n";
			getcpuname(buffer2);
			strcat(string, buffer2);
			write(new_socket, string, strlen(string));
		}
		else if (strstr(buffer, "GET /load"))
		{

			
			int cpu = getcpuload();
			char string[1256] = "HTTP/1.1 200 OK\nContent-Type: text/plain;\r\n\r\n";
			char str[3];
			sprintf(str, "%d", cpu);
			strcat(string, str);
			strcat(string, "%\n");
			write(new_socket, string, strlen(string));
		}
		else
		{
			
			char string[1256] = "HTTP/1.1 400 Bad Request\r\n";
			write(new_socket, string, strlen(string));
		}

		close(new_socket);
	}
	return 0;
}
