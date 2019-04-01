#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "rpc_types.h"

#define SOCK_PATH SERVER_PATH
#define SERVER_PATH "/var/tmp/tpf_unix_sock.server"

int main(void)
{
    int server_sock, client_sock, len, rc;
    int bytes_rec = 0;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    char buf[256];
    int backlog = 10;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, 256);

    /* Create a UNIX domain stream socket */
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1)
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
    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, SOCK_PATH);
    len = sizeof(server_sockaddr);

    unlink(SOCK_PATH);
    rc = bind(server_sock, (struct sockaddr *)&server_sockaddr, len);
    if (rc == -1)
    {
        printf("BIND ERROR\n");
        close(server_sock);
        exit(1);
    }

    /* Listen for any client sockets */
    rc = listen(server_sock, backlog);
    if (rc == -1)
    {
        printf("LISTEN ERROR\n");
        close(server_sock);
        exit(1);
    }
    printf("socket listening...\n");

    /* Accept an incoming connection */
    client_sock = accept(server_sock, (struct sockaddr *)&client_sockaddr, &len);
    if (client_sock == -1)
    {
        printf("ACCEPT ERROR\n");
        close(server_sock);
        close(client_sock);
        exit(1);
    }

    /* Get the name of the connected socket */
    len = sizeof(client_sockaddr);
    rc = getpeername(client_sock, (struct sockaddr *)&client_sockaddr, &len);
    if (rc == -1)
    {
        printf("GETPEERNAME ERROR\n");
        close(server_sock);
        close(client_sock);
        exit(1);
    }
    else
    {
        printf("Client socket filepath: %s\n", client_sockaddr.sun_path);
    }

    /* Read and print the data incoming on the connected socket */
    printf("waiting to read...\n");
    struct para_join para;
    bytes_rec = recv(client_sock, &para, sizeof(struct para_join), 0);
    if (bytes_rec == -1)
    {
        printf("RECV ERROR\n");
        close(server_sock);
        close(client_sock);
        exit(1);
    }
    else
    {
        printf("DATA RECEIVED\n");
        printf("\tfunc_code: %d\n", para.func_code);
        printf("\teth_port: %d\n", para.eth_port);
        printf("\tib_port: %d\n", para.ib_port);
        printf("\tinput_str: %s\n", para.input_str);
    }

    /* Send data back to the connected socket */
    int rsp = 12;
    printf("Sending data...\n");
    rc = send(client_sock, &rsp, sizeof(int), 0);
    if (rc == -1)
    {
        printf("SEND ERROR\n");
        close(server_sock);
        close(client_sock);
        exit(1);
    }
    else
    {
        printf("Data sent!\n");
    }

    /* Close the sockets and exit */
    close(server_sock);
    close(client_sock);

    return 0;
}