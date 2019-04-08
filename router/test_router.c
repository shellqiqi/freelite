#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <unistd.h>

#include "../inc/func_code.h"
#include "../inc/rpc_types.h"
#include "../inc/lite-lib.h"

#include "lib/shm.h"

#define SERVER_PATH "/var/tmp/tpf_unix_sock.server"

void *server_accept_request(void *fd)
{
    socklen_t len;
    int client_sock = *((int *)fd);
    struct rpc_req_msg req_msg;
    struct rpc_rsp_msg rsp_msg;
    struct sockaddr_un client_sockaddr;
    memset(&req_msg, 0, sizeof(struct rpc_req_msg));
    memset(&rsp_msg, 0, sizeof(struct rpc_rsp_msg));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

    /* Get the name of the connected socket */
    len = sizeof(client_sockaddr);
    if (getpeername(client_sock, (struct sockaddr *)&client_sockaddr, &len) < 0)
    {
        perror("GETPEERNAME ERROR\n");
        close(client_sock);
        pthread_exit(NULL);
    }
    printf("Client socket filepath: %s\n", client_sockaddr.sun_path);

    /* Read and print the data incoming on the connected socket */
    printf("waiting to read...\n");
    if (recv(client_sock, &req_msg, sizeof(struct rpc_req_msg), 0) < 0)
    {
        perror("RECV ERROR\n");
        close(client_sock);
        pthread_exit(NULL);
    }

    printf("DATA RECEIVED\n");
    printf("  func_code: %d\n", req_msg.func_code);
    switch (req_msg.func_code)
    {
    case FUNC_userspace_liteapi_get_node_id:
        rsp_msg.rval.int_rsp = userspace_liteapi_get_node_id();
        break;
    case FUNC_userspace_liteapi_join:
        printf("  eth_port: %d\n", req_msg.msg_body.join_req.eth_port);
        printf("  ib_port: %d\n", req_msg.msg_body.join_req.ib_port);
        printf("  input_str: %s\n", req_msg.msg_body.join_req.input_str);
        rsp_msg.rval.int_rsp = userspace_liteapi_join(req_msg.msg_body.join_req.input_str,
                                                      req_msg.msg_body.join_req.eth_port,
                                                      req_msg.msg_body.join_req.ib_port);
        break;
    case FUNC_userspace_liteapi_alloc_remote_mem:
        printf("  node_id: %d\n", req_msg.msg_body.alloc_remote_mem_req.node_id);
        printf("  size: %d\n", req_msg.msg_body.alloc_remote_mem_req.size);
        printf("  atomic_flag: %d\n", req_msg.msg_body.alloc_remote_mem_req.atomic_flag);
        printf("  password: %d\n", req_msg.msg_body.alloc_remote_mem_req.password);
        rsp_msg.rval.int_rsp = userspace_liteapi_alloc_remote_mem(req_msg.msg_body.alloc_remote_mem_req.node_id,
                                                                  req_msg.msg_body.alloc_remote_mem_req.size,
                                                                  req_msg.msg_body.alloc_remote_mem_req.atomic_flag,
                                                                  req_msg.msg_body.alloc_remote_mem_req.password);
        break;
    case FUNC_userspace_liteapi_rdma_write: //TODO: the local_addr is alloced by router and passes to client?
        printf("  lite_handler: %d\n", req_msg.msg_body.rdma_write_req.lite_handler);
        printf("  local_addr: %p\n", req_msg.msg_body.rdma_write_req.local_addr);
        printf("  size: %d\n", req_msg.msg_body.rdma_write_req.size);
        printf("  offset: %d\n", req_msg.msg_body.rdma_write_req.offset);
        printf("  password: %d\n", req_msg.msg_body.rdma_write_req.password);
        rsp_msg.rval.int_rsp = userspace_liteapi_rdma_write(req_msg.msg_body.rdma_write_req.lite_handler,
                                                            req_msg.msg_body.rdma_write_req.local_addr,
                                                            req_msg.msg_body.rdma_write_req.size,
                                                            req_msg.msg_body.rdma_write_req.offset,
                                                            req_msg.msg_body.rdma_write_req.password);
        break;
    case FUNC_userspace_liteapi_rdma_read:
        printf("  lite_handler: %d\n", req_msg.msg_body.rdma_read_req.lite_handler);
        printf("  local_addr: %p\n", req_msg.msg_body.rdma_read_req.local_addr);
        printf("  size: %d\n", req_msg.msg_body.rdma_read_req.size);
        printf("  offset: %d\n", req_msg.msg_body.rdma_read_req.offset);
        printf("  password: %d\n", req_msg.msg_body.rdma_read_req.password);
        rsp_msg.rval.int_rsp = -2;
        break;
    case FUNC_userspace_liteapi_send_reply_imm_fast:
        printf("  target_node: %d\n", req_msg.msg_body.send_reply_imm_fast_req.target_node);
        printf("  port: %d\n", req_msg.msg_body.send_reply_imm_fast_req.port);
        printf("  addr: %p\n", req_msg.msg_body.send_reply_imm_fast_req.addr);
        printf("  size: %d\n", req_msg.msg_body.send_reply_imm_fast_req.size);
        printf("  ret_addr: %p\n", req_msg.msg_body.send_reply_imm_fast_req.ret_addr);
        printf("  ret_length: %p\n", req_msg.msg_body.send_reply_imm_fast_req.ret_length);
        printf("  max_ret_size: %d\n", req_msg.msg_body.send_reply_imm_fast_req.max_ret_size);
        rsp_msg.rval.int_rsp = 1024;
        break;
    case FUNC_userspace_liteapi_receive_message_fast:
        printf("  port: %d\n", req_msg.msg_body.receive_message_fast_req.port);
        printf("  ret_addr: %p\n", req_msg.msg_body.receive_message_fast_req.ret_addr);
        printf("  receive_size: %d\n", req_msg.msg_body.receive_message_fast_req.receive_size);
        printf("  descriptor: %p\n", req_msg.msg_body.receive_message_fast_req.descriptor);
        printf("  ret_length: %p\n", req_msg.msg_body.receive_message_fast_req.ret_length);
        printf("  block_call: %d\n", req_msg.msg_body.receive_message_fast_req.block_call);
        rsp_msg.rval.int_rsp = 512;
        break;
    case FUNC_userspace_liteapi_reply_message:
        printf("  port: %p\n", req_msg.msg_body.reply_message_req.addr);
        printf("  ret_addr: %d\n", req_msg.msg_body.reply_message_req.size);
        printf("  receive_size: %lu\n", req_msg.msg_body.reply_message_req.descriptor);
        rsp_msg.rval.int_rsp = -4;
        break;
    case FUNC_userspace_liteapi_register_application:
        printf("  destined_port: %d\n", req_msg.msg_body.register_application_req.destined_port);
        printf("  max_size_per_message: %d\n", req_msg.msg_body.register_application_req.max_size_per_message);
        printf("  max_user_per_node: %d\n", req_msg.msg_body.register_application_req.max_user_per_node);
        printf("  name: %s\n", req_msg.msg_body.register_application_req.name);
        printf("  name_len: %lu\n", req_msg.msg_body.register_application_req.name_len);
        rsp_msg.rval.int_rsp = -10;
        break;
    case FUNC_userspace_liteapi_dist_barrier:
        printf("  num: %d\n", req_msg.msg_body.dist_barrier_req.num);
        rsp_msg.rval.int_rsp = -7;
        break;
    case FUNC_userspace_liteapi_query_port:
        printf("  target_node: %d\n", req_msg.msg_body.query_port_req.target_node);
        printf("  designed_port: %d\n", req_msg.msg_body.query_port_req.designed_port);
        rsp_msg.rval.int_rsp = -9;
        break;
    case FUNC_userspace_liteapi_alloc_local_mem:
        printf("  name: %s\n", req_msg.msg_body.alloc_local_mem_req.name);
        printf("  size: %lu\n", req_msg.msg_body.alloc_local_mem_req.size);
        rsp_msg.rval.int_rsp = alloc_shm_mem(req_msg.msg_body.alloc_local_mem_req.name,
                                             req_msg.msg_body.alloc_local_mem_req.size,
                                             &rsp_msg.msg_body.alloc_local_mem_rsp.remote_addr);
        break;
    case FUNC_userspace_liteapi_free_local_mem:
        printf("  name: %s\n", req_msg.msg_body.free_local_mem_req.name);
        printf("  size: %lu\n", req_msg.msg_body.free_local_mem_req.size);
        printf("  remote_addr: %p\n", req_msg.msg_body.free_local_mem_req.remote_addr);
        rsp_msg.rval.int_rsp = free_shm_mem(req_msg.msg_body.free_local_mem_req.name,
                                            req_msg.msg_body.free_local_mem_req.size,
                                            req_msg.msg_body.free_local_mem_req.remote_addr);
        break;
    default:
        perror("UNKNOWN FUNC CODE\n");
        close(client_sock);
        pthread_exit(NULL);
    }

    /* Send data back to the connected socket */
    printf("Sending data...\n");
    if (send(client_sock, &rsp_msg, sizeof(struct rpc_rsp_msg), 0) < 0)
    {
        printf("SEND ERROR\n");
        close(client_sock);
        pthread_exit(NULL);
    }
    printf("Data sent!\n");

    /* Close the sockets and exit */
    close(client_sock);
    pthread_exit(NULL);
}

int main(void)
{
    int server_sock, client_sock;
    socklen_t len;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    int backlog = 10;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

    /* Create a UNIX domain stream socket */
    if ((server_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
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
    strcpy(server_sockaddr.sun_path, SERVER_PATH);
    len = sizeof(server_sockaddr);

    unlink(SERVER_PATH);
    if (bind(server_sock, (struct sockaddr *)&server_sockaddr, len) < 0)
    {
        perror("BIND ERROR\n");
        close(server_sock);
        exit(1);
    }

    /* Listen for any client sockets */
    if (listen(server_sock, backlog) < 0)
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
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_sockaddr, &len)) < 0)
        {
            perror("ACCEPT ERROR\n");
            close(server_sock);
            close(client_sock);
            exit(1);
        }
        if (pthread_create(&pth, NULL, server_accept_request, &client_sock) < 0)
        {
            perror("THREAD CREATE ERROR\n");
            exit(1);
        }
    }

    /* Close the sockets and exit */
    close(server_sock);
    close(client_sock);

    return 0;
}