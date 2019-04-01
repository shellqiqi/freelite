#include "rpc.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_PATH "/var/tmp/tpf_unix_sock.server"

#define CLIENT_PATH "/var/tmp/" /* +5 for pid = 14 chars */

/* PRIVATE VARIABLES */

/* socket addresses of server and client */
static struct sockaddr_un server_sockaddr;
static struct sockaddr_un client_sockaddr;

/* PRIVATE FUNCTIONS */

/**
 * Connect server.
 * Input:
 *    name: server name.
 * Return: client file descriptor.
 */
static int conn(const char *name)
{
    int client_sock, rc, len;

    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

    /* Create a UNIX domain stream socket */
    if ((client_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        printf("SOCKET ERROR\n");
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
        printf("BIND ERROR\n");
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
        printf("CONNECT ERROR\n");
        close(client_sock);
        exit(1);
    }

    return client_sock;
}

static void disconn(int client_sock)
{
    close(client_sock);
}

/* PUBLIC FUNCTIONS */

int rpc_get_node_id()
{
    int rc;
    int func_code = FUNC_userspace_liteapi_get_node_id;

    int client_sock = conn(SERVER_PATH);

    rc = send(client_sock, &func_code, sizeof(int), 0);
    if (rc == -1)
    {
        printf("SEND ERROR\n");
        close(client_sock);
        exit(1);
    }
    else
    {
        printf("Data sent!\n");
    }

    /* Read the data sent from the server and print it. */
    int rsp;
    rc = recv(client_sock, &rsp, sizeof(int), 0);
    if (rc == -1)
    {
        printf("RECV ERROR\n");
        close(client_sock);
        exit(1);
    }
    else
    {
        printf("DATA RECEIVED = %d\n", rsp);
    }

    /* Close the socket and exit. */
    close(client_sock);

    return rsp;
}

int rpc_join(const struct para_join *para)
{
    int rc;

    int client_sock = conn(SERVER_PATH);

    rc = send(client_sock, para, sizeof(struct para_join), 0);
    if (rc == -1)
    {
        printf("SEND ERROR\n");
        close(client_sock);
        exit(1);
    }
    else
    {
        printf("Data sent!\n");
    }

    /* Read the data sent from the server and print it. */
    int rsp;
    rc = recv(client_sock, &rsp, sizeof(int), 0);
    if (rc == -1)
    {
        printf("RECV ERROR\n");
        close(client_sock);
        exit(1);
    }
    else
    {
        printf("DATA RECEIVED = %d\n", rsp);
    }

    /* Close the socket and exit. */
    close(client_sock);

    return rsp;
}