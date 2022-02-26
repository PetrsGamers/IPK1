#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#define PORT 8080
char *gethostname(char *buffer)
{
    FILE *fptr;
    char *path[255];
    *path = "cat /proc/sys/kernel/hostname";
    fptr = popen(*path, "r");
    fgets(buffer, 256, fptr);
    printf("%s   %i", buffer, strlen(buffer));
    return;
}
void getcpuname(char *buffer)
{
    FILE *fptr;
    char *path[255];
    *path = "cat /proc/cpuinfo | grep 'model name' | head -n 1 | cut -d':' -f2-";
    fptr = popen(*path, "r");
    fgets(buffer, 256, fptr);
    printf("%s   %i", buffer, strlen(buffer));
    return;
}
void getcpuload(char *buffer)
{
    FILE *fptr;
    char *path[255];
    *path = "cat /proc/cpuinfo | grep 'model name' | head -n 1 | cut -d':' -f2-";
    fptr = popen(*path, "r");
    fgets(buffer, 256, fptr);
    printf("%s   %i", buffer, strlen(buffer));
    return;
}
int main(int argc, char const *argv[])
{
    if (argc == 1 || argc > 2)
    {
        return 1;
    }
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Only this line has been changed. Everything is same.
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\n";
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
    {
        perror("In socket");
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
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        char buffer2[256];
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        char buffer[30000] = {0};
        valread = read(new_socket, buffer, 30000);
        if (strstr(buffer, "/hostname"))
        {
            gethostname(buffer2);
        }
        if (strstr(buffer, "/cpu-name"))
        {
            getcpuname(buffer2);
        }
		if (strstr(buffer, "/load"))
		{

			getcpuload(buffer);
		}
		else
		{

		}
        char string[256] = "HTTP/1.1 200 OK\nContent-Type: text/plain;\r\n\r\n";
        strcat(string, buffer2);
        write(new_socket, string, strlen(string));

        close(new_socket);
    }
    return 0;
}
