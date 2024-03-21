// rchaney@pdx.edu

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>
#include <pthread.h>

#include "rockem_hdr.h"

static unsigned short is_verbose = 0;
static unsigned sleep_flag = 0;

static char ip_addr[50] = "131.252.208.23";
static short ip_port = DEFAULT_SERV_PORT;

int get_socket(char *, int);
void get_file(char *);
void put_file(char *);
void *thread_get(void *);
void *thread_put(void *);
void list_dir(void);

int
main(int argc, char *argv[])
{
    cmd_t cmd;
    int opt;
    int i;
	pthread_t tid;	

    memset(&cmd, 0, sizeof(cmd_t));
    while ((opt = getopt(argc, argv, CLIENT_OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            // copy optarg into the ip_addr
			strcpy(ip_addr, optarg);
            break;
        case 'p':
            // CONVERT and assign optarg to ip_port
			sscanf(optarg, "%hd", &ip_port);
            break;
        case 'c':
            // copy optarg into data member cmd.cmd
			strcpy(cmd.cmd, optarg);
            break;
        case 'v':
            is_verbose++;
            break;
        case 'u':
            // add 1000 to sleep_flag
			sleep_flag += 1000;
            break;
        case 'h':
            fprintf(stderr, "%s ...\n\tOptions: %s\n"
                    , argv[0], CLIENT_OPTIONS);
            fprintf(stderr, "\t-i str\t\tIPv4 address of the server (default %s)\n"
                    , ip_addr);
            fprintf(stderr, "\t-p #\t\tport on which the server will listen (default %hd)\n"
                    , DEFAULT_SERV_PORT);
            fprintf(stderr, "\t-c str\t\tcommand to run (one of %s, %s, or %s)\n"
                    , CMD_GET, CMD_PUT, CMD_DIR);
            fprintf(stderr, "\t-u\t\tnumber of thousands of microseconds the client will sleep between read/write calls (default %d)\n"
                    , 0);
            fprintf(stderr, "\t-v\t\tenable verbose output. Can occur more than once to increase output\n");
            fprintf(stderr, "\t-h\t\tshow this rather lame help message\n");
            exit(EXIT_SUCCESS);
            break;
        default:
            fprintf(stderr, "*** Oops, something strange happened <%s> ***\n", argv[0]);
            break;
        }
    }

    if (is_verbose) {
        fprintf(stderr, "Command to server: <%s> %d\n"
                , cmd.cmd, __LINE__);
    }
    if (strcmp(cmd.cmd, CMD_GET) == 0) {
        // process the files left on the command line, creating a threas for
        // each file to connect to the server
        for (i = optind; i < argc; i++) {
            // create threads
            // pass the file name as the ONE parameter to the thread function
			pthread_create(&tid, NULL, thread_get, (void *) argv[i]);
        }
    }
    else if (strcmp(cmd.cmd, CMD_PUT) == 0) {
        // process the files left on the command line, creating a threas for
        // each file to connect to the server
        for (i = optind; i < argc; i++) {
            // create threads
            // pass the file name as the ONE parameter to the thread function
			pthread_create(&tid, NULL, thread_put, (void *) argv[i]);
        }
    }
    else if (strcmp(cmd.cmd, CMD_DIR) == 0) {
        list_dir();
    }
    else {
        fprintf(stderr, "ERROR: unknown command >%s< %d\n", cmd.cmd, __LINE__);
        exit(EXIT_FAILURE);
    }

    pthread_exit(NULL);
}

int
get_socket(char * addr, int port)
{
    // configure and create a new socket for the connection to the server
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
	
    // more stuff in here
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(ip_port);
	inet_pton(AF_INET, ip_addr, &servaddr.sin_addr.s_addr);

	if(connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0)
	{
		perror("could not connect");
		exit(EXIT_FAILURE);
	}	
    return(sockfd);
}

// get one file
void
get_file(char *file_name)
{
    cmd_t cmd;
    int sockfd;
    int fd;
    ssize_t bytes_read;
    char buffer[MAXLINE] = {'\0'};

	memset(&cmd, 0, sizeof(cmd_t));
    
	strcpy(cmd.cmd, CMD_GET);
    if (is_verbose) {
        fprintf(stderr, "next file: <%s> %d\n", file_name, __LINE__);
    }
    strcpy(cmd.name, file_name);
    if (is_verbose) {
        fprintf(stderr, "get from server: %s %s %d\n", cmd.cmd, cmd.name, __LINE__);
    }

    // get the new socket to the server (get_socket(...)
    sockfd = get_socket(ip_addr, ip_port);
   
	// write the command to the socket
	write(sockfd, &cmd, sizeof(cmd));
    
    // open the file to write
	fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd < 0)
	{
		perror("open failed");
		return;
	}

    // loop reading from the socket, writing to the file
    //   until the socket read is zero
	while((bytes_read = read(sockfd, buffer, MAXLINE)) != 0)
	{
		if(sleep_flag > 0) usleep(sleep_flag);
		write(fd, buffer, bytes_read);
	}

    // close the file
	close(fd);
    // close the socket
	close(sockfd);
}

void
put_file(char *file_name)
{
    cmd_t cmd;
    int sockfd;
    int fd;
    ssize_t bytes_read;
    char buffer[MAXLINE] = {'\0'};
	
	memset(&cmd, 0, sizeof(cmd_t));

    strcpy(cmd.cmd, CMD_PUT);
    if (is_verbose) {
        fprintf(stderr, "next file: <%s> %d\n", file_name, __LINE__);
    }
    strcpy(cmd.name, file_name);
    if (is_verbose) {
        fprintf(stderr, "put to server: %s %s %d\n", cmd.cmd, cmd.name, __LINE__);
    }
		
    // get the new socket to the server (get_socket(...)
	sockfd = get_socket(ip_addr, ip_port);

    // write the command to the socket
	write(sockfd, &cmd, sizeof(cmd_t));

    // open the file for read
	fd = open(file_name, O_RDONLY);
	if(fd < 0)
	{
		perror("open failed");
		return;
	}
    // loop reading from the file, writing to the socket
    //   until file read is zero
	while((bytes_read = read(fd, buffer, MAXLINE)) != 0)
	{
		if(sleep_flag > 0) usleep(sleep_flag);
		write(sockfd, buffer, bytes_read);
	}
    
	// close the file
	close(fd);
    // close the socket
	close(sockfd);
}

void
list_dir(void)
{
    cmd_t cmd;
    int sockfd;
    ssize_t bytes_read;
    char buffer[MAXLINE] = {'\0'};

    printf("dir from server: %s \n", ip_addr);

    // get the new socket to the server (get_socket(...)
    sockfd = get_socket(ip_addr, ip_port);

    strcpy(cmd.cmd, CMD_DIR);

    // write the command to the socket 
	write(sockfd, &cmd, sizeof(cmd));
    
	// loop reading from the socket, writing to the file
    //   until the socket read is zero
	while((bytes_read = read(sockfd, buffer, MAXLINE)) != 0)
	{
		write(STDOUT_FILENO, buffer, bytes_read);
	}
	
	close(sockfd);
    // close the socket
}

void *
thread_get(void *info)
{
    char *file_name = (char *) info;

    // detach this thread 'man pthread_detach' Look at the EXMAPLES
	pthread_detach(pthread_self());

    // process one file
    get_file(file_name);

    pthread_exit(NULL);
}

void *
thread_put(void *info)
{
    char *file_name = (char *) info;

    // detach this thread 'man pthread_detach' Look at the EXMAPLES
	pthread_detach(pthread_self());

    // process one file
    put_file(file_name);

    pthread_exit(NULL);
}
