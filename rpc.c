#include "rpc.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_PATH "tpf_unix_sock.server"
#define DATA "Hello from client"

#define CLIENT_PATH "/var/tmp/" /* +5 for pid = 14 chars */

/* PRIVATE VARIABLES */

/* socket addresses of server and client */
static struct sockaddr_un server_sockaddr;
static struct sockaddr_un client_sockaddr;

/* PUBLIC FUNCTIONS */

int test(void)
{
    int rc;
    char buf[256];

    int client_sock = conn(SERVER_PATH);

    /* Copy the data to the buffer and end it to the server socket. */
    strcpy(buf, DATA);
    printf("Sending data...\n");
    rc = send(client_sock, buf, strlen(buf), 0);
    if (rc == -1)
    {
        printf("SEND ERROR = %d\n", sock_errno());
        close(client_sock);
        exit(1);
    }
    else
    {
        printf("Data sent!\n");
    }

    /* Read the data sent from the server and print it. */
    printf("Waiting to recieve data...\n");
    memset(buf, 0, sizeof(buf));
    rc = recv(client_sock, buf, sizeof(buf), 0);
    if (rc == -1)
    {
        printf("RECV ERROR = %d\n", sock_errno());
        close(client_sock);
        exit(1);
    }
    else
    {
        printf("DATA RECEIVED = %s\n", buf);
    }

    /* Close the socket and exit. */
    close(client_sock);

    return 0;
}

/**
 * Connect server.
 * Input:
 *    name: server name.
 * Return: client file descriptor.
 */
int conn(const char *name)
{
    int client_sock, rc, len;

    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

    /* Create a UNIX domain stream socket */
    if ((client_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        printf("SOCKET ERROR = %d\n", sock_errno());
        exit(1);
    }

    /**
     * Set up the UNIX sockaddr structure
     * by using AF_UNIX for the family and
     * giving it a filepath to bind to.
     *
     * Unlink the file so the bind will
     * succeed, then bind to that file.
     */
    client_sockaddr.sun_family = AF_UNIX;
    sprintf(client_sockaddr.sun_path, "%s%05d", CLIENT_PATH, getpid());
    len = sizeof(client_sockaddr);

    unlink(client_sockaddr.sun_path);
    if ((bind(client_sock, (struct sockaddr *)&client_sockaddr, len)) == -1)
    {
        printf("BIND ERROR: %d\n", sock_errno());
        close(client_sock);
        exit(1);
    }

    /**
     * Set up the UNIX sockaddr structure
     * for the server socket and connect
     * to it.
     */
    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, name);
    rc = connect(client_sock, (struct sockaddr *)&server_sockaddr, len);
    if (rc == -1)
    {
        printf("CONNECT ERROR = %d\n", sock_errno());
        close(client_sock);
        exit(1);
    }

    return client_sock;
}

void disconn(int client_sock)
{
    close(client_sock);
}