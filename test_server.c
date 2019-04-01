#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

#include "inc/rpc_types.h"

#define SOCK_PATH SERVER_PATH
#define SERVER_PATH "/var/tmp/tpf_unix_sock.server"

void *server_accept_request(void *fd)
{
    int len, rc, bytes_rec;
    int client_sock = *((int *)fd);

    struct sockaddr_un client_sockaddr;
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

    /* Get the name of the connected socket */
    len = sizeof(client_sockaddr);
    rc = getpeername(client_sock, (struct sockaddr *)&client_sockaddr, &len);
    if (rc == -1)
    {
        perror("GETPEERNAME ERROR\n");
        close(client_sock);
        pthread_exit(NULL);
    }
    else
    {
        printf("Client socket filepath: %s\n", client_sockaddr.sun_path);
    }

    /* Read and print the data incoming on the connected socket */
    printf("waiting to read...\n");
    struct rpc_req_msg req_msg;
    bytes_rec = recv(client_sock, &req_msg, sizeof(struct rpc_req_msg), 0);
    if (bytes_rec == -1)
    {
        perror("RECV ERROR\n");
        close(client_sock);
        pthread_exit(NULL);
    }
    else
    {
        printf("DATA RECEIVED\n");
        printf("\tfunc_code: %d\n", req_msg.func_code);
        printf("\teth_port: %d\n", req_msg.msg_body.join_req.eth_port);
        printf("\tib_port: %d\n", req_msg.msg_body.join_req.ib_port);
        printf("\tinput_str: %s\n", req_msg.msg_body.join_req.input_str);
    }

    /* Send data back to the connected socket */
    struct rpc_rsp_msg rsp_msg = {
        .msg_body.int_rval = 12
    };
    printf("Sending data...\n");
    rc = send(client_sock, &rsp_msg, sizeof(struct rpc_rsp_msg), 0);
    if (rc == -1)
    {
        printf("SEND ERROR\n");
        close(client_sock);
        pthread_exit(NULL);
    }
    else
    {
        printf("Data sent!\n");
    }

    /* Close the sockets and exit */
    close(client_sock);
    pthread_exit(NULL);
}

int main(void)
{
    int server_sock, client_sock, len, rc;
    int bytes_rec = 0;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    char buf[256];
    int backlog = 10;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));

    /* Create a UNIX domain stream socket */
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        perror("SOCKET ERROR\n");
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
    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, SOCK_PATH);
    len = sizeof(server_sockaddr);

    unlink(SOCK_PATH);
    rc = bind(server_sock, (struct sockaddr *)&server_sockaddr, len);
    if (rc == -1)
    {
        perror("BIND ERROR\n");
        close(server_sock);
        exit(1);
    }

    /* Listen for any client sockets */
    rc = listen(server_sock, backlog);
    if (rc == -1)
    {
        perror("LISTEN ERROR\n");
        close(server_sock);
        exit(1);
    }
    printf("socket listening...\n");

    pthread_t pth;
    while (1)
    {
        /* Accept an incoming connection */
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_sockaddr, &len)) == -1)
        {
            perror("ACCEPT ERROR\n");
            close(server_sock);
            close(client_sock);
            exit(1);
        }
        if (pthread_create(&pth, NULL, server_accept_request, &client_sock) < 0)
        {
            perror("THREAD CREATE ERROR\n");
        }
    }

    /* Close the sockets and exit */
    close(server_sock);
    close(client_sock);

    return 0;
}