#include "rpc.h"
#include "../../inc/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SERVER_PATH "/var/tmp/tpf_unix_sock.server"

#define CLIENT_PATH "/var/tmp/" /* +5 for pid = 14 chars */

/* Reused socket fd */

static int client_sock = -1;

/* PUBLIC FUNCTIONS */

/**
 * Connect server.
 * Input:
 *    name: server name.
 * Return: error code.
 */
int rpc_conn(const char *name)
{
    int len;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

    /* Create a UNIX domain stream socket */
    if ((client_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        LOG_PERROR("SOCKET ERROR");
        return -1;
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
    if (bind(client_sock, (struct sockaddr *)&client_sockaddr, len) < 0)
    {
        LOG_PERROR("BIND ERROR");
        rpc_disconn();
        return -1;
    }

    /**
     * Set up the UNIX sockaddr structure
     * for the server socket and connect
     * to it.
     */
    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, name);
    if (connect(client_sock, (struct sockaddr *)&server_sockaddr, len) < 0)
    {
        LOG_PERROR("CONNECT ERROR");
        rpc_disconn();
        return -1;
    }

    return client_sock;
}

/**
 * Disconnect router
 */
void rpc_disconn()
{
    close(client_sock);
    client_sock = -1;
}

/**
 * Handle rpc
 * Input:
 *    req_msg: request message structure
 * Output:
 *    rsp_msg: response message structure
 * Return: error code
 */
int rpc_handler(const struct rpc_req_msg *req_msg,
                struct rpc_rsp_msg *rsp_msg)
{
    if (client_sock == -1)             // Not connected yet
        if (rpc_conn(SERVER_PATH) < 0) // connect
            return -1;

    if (send(client_sock, req_msg, sizeof(struct rpc_req_msg), 0) < 0)
    {
        LOG_PERROR("SEND ERROR");
        rpc_disconn();
        return -1;
    }
    LOG_INFO("Data sent!\n");

    /* Read the data sent from the server and print it. */
    if (recv(client_sock, rsp_msg, sizeof(struct rpc_rsp_msg), 0) < 0)
    {
        LOG_PERROR("RECV ERROR");
        rpc_disconn();
        return -1;
    }
    LOG_INFO("DATA RECEIVED\n");

    return 0;
}
