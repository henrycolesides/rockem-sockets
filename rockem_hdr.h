// rchaney@pdx.edu

#pragma once

#ifndef SOCKET_HDR_H
# define SOCKET_HDR_H 1

#ifndef TRUE
# define TRUE 1
#endif // TRUE
#ifndef FALSE
# define FALSE 0
#endif // FALSE

# define MAXLINE 1024
// The size of the buffer used to transfer data from/to the server. This makes
//   it easy for me to display how much data have been moved from/to the server.
// We want this value to be larger than sizeof(cmd_t).

# define DEFAULT_SERV_PORT 10001
// The default port on which the server will try to listen for incomming
//   client connections. You may/will need to make use of the command line option
//   to alter this when you are testing your code.

// The following structure can be used when a client connects to the server. A client
//   would fill out the structure as something like:
//     cmd:  get
//     name: zeroes5k.dat
// The client would connect to the server and write the structure to the socket.
// The server would then write the data from the zeroes5k.dat file back to the
//   client over the socket connection with the client. The client would create and
//   store the file in its directory. Once the data transfer is complete, the server
//   will close the socket, making it easy for the client to know when the transfer
//   of a file is compelte.
//
// Another example, 
//   cmd:  put
//   name: zeroes5m_Test1.dat
// The client would connect to the server and write the structure to the socket.
// The client would write the data from the zeroes5m_Test1.dat file to the server,
//   which would create and store the file in its directory. Once the data transfer
//   is complete, the client will close the socket, making it easy for the server to
//   know when the transfer is compelte.
//
// Last example:
//   cmd:  dir
// The server use the popen() command to send a listing of it current directory back
//   to the client.

# define CMD_LEN 7
# define NAME_LEN 75

// This is a structure I use when the client sents a command to the server process.
// I use the fixed length structure.
// The string for the command is in the cmd data member. It is short. Only 4 characters
//   are actually needed (including the NULL terminator).
// If the command is a put or get, then the name data member contains the name of the
//   file to be sent to the sever (for a put) or fetched from the server (for e get).
// For the messages sent over the socket, the sock data member is unused.
// I also use this sctructure within the server process, I pass it as the single parameter
//   to the thread created to handle the command. In place the socket from the newly created
//   socket connection in the socket data member.
typedef struct cmd_s {
    char cmd[CMD_LEN];
    // The command that a client sends to the server.

    char name[NAME_LEN];
    // The file name that a client sends to or fetches from the server.

    // I use this structure in the server process to provide data to the
    // threads that manage the connections with the clients.
    int sock;
    // The socket returned from the accept() call in the server when a new
    // client connects to the server.
} cmd_t;

// Fetch a file from the server to the client.
# define CMD_GET "get"

// Copy a file from the client to the server.
# define CMD_PUT "put"

// Return a directory listing from the server's working directory.
# define CMD_DIR  "dir"

# define CLIENT_OPTIONS "i:p:c:uvh"
// The command line options for the client are:
//    i: identify the ip address where the server is running.
//       This should an IPv4 style address.
//    p: The port on which the server process is listening.
//    c: The command the client wishes to send to the server.
//       The choices are
//           get: fetch each file from the server to the client
//           put: copy each file from the client to the server
//           dir: fetch a directory listing from the server.
//                The server can only return the directory listing
//                where it was started.
//                Make use of the popen() command.
//    u: add a 1000 microsecond delay after each read/write on
//       the socket connection to the server. Adding the delay
//       makes it easier to see how the server is managing multiple
//       concurrent connections.
//       This is optional for the client, but required for the server.
//    v: Display a dot for each 1k bytes read from or written to the
//       server. This will allow yout to see how the server is
//       handeling multiple client connections.
//       This is not an optional command line option for the client,
//       your client must accept and perform this action.
//    h: the usual helpful message option.
//       This command line option is optional.

# define SERVER_OPTIONS "p:uvh"
// The command line options for the server are:
//    p: The port on which the server process is listening.
//    u: add a 1000 microsecond delay after each read/write on
//       the socket connection to a client. Adding the delay
//       makes it easier to see how the server is managing multiple
//       concurrent connections.
//       This is optional for the client, but required for the server.
//    v: The usual verbose processing option.
//       This command line option is optional for the server.
//       Though optional, this is a very helpful option. I display
//       a decimal for each 1k of data transfered. Each connection
//       is given a unique integer, starting at 1 and going up.
//    h: the usual helpful message option.
//       This command line option is optional.

// This is the ls command you can use with a call of popen() to
//   return the directory listing on the server, from its current
//   working directory. There is not a method in this to allow the
//   server to change directories. Consult the man page for ls to
//   see what all the options mean.
# define CMD_DIR_POPEN    "ls -lFABhS --si --group-directories-first"

// The interactive commands for the server terminal.
// count: display the total connections that have been made to the server since startup
//        display the number of current live connections on the server
//        display the verbose setting
// v+   : increment the verbose setting.
// v-   : decrement the verbose setting.
// helo : display the available commands.
// exit : exit the server, killing any corrent connections.
# define SERVER_CMD_COUNT    "count"
# define SERVER_CMD_VPLUS    "v+"
# define SERVER_CMD_VMINUS   "v-"
# define SERVER_CMD_HELP     "help"
# define SERVER_CMD_EXIT     "exit"

#endif // SOCKET_HDR_H
