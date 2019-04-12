#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include "../inc/func_code.h"
#include "../inc/rpc_types.h"
#include "../inc/lite-lib.h"
#include "../inc/log.h"

#include "lib/shm.hpp"
#include "lib/routing.hpp"
#include "config.hpp"

static bool keepRunning = true;

int router_nid = -1;

void INThandler(int sig)
{
    keepRunning = false;
}

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
        LOG_PERROR("GETPEERNAME ERROR");
        close(client_sock);
        pthread_exit(NULL);
    }
    LOG_DEBUG("Client socket filepath: %s\n", client_sockaddr.sun_path);

    while (1)
    {
        /* Read and print the data incoming on the connected socket */
        LOG_INFO("waiting to read...\n");
        int ret_len = recv(client_sock, &req_msg, sizeof(struct rpc_req_msg), 0);
        if (ret_len < 0)
        {
            LOG_PERROR("RECV ERROR");
            close(client_sock);
            pthread_exit(NULL);
        }
        else if (ret_len == 0)
        {
            LOG_WARN("CLIENT SHUTDOWN\n");
            close(client_sock);
            pthread_exit(NULL);
        }

        LOG_INFO("DATA RECEIVED\n");
        LOG_INFO("  func_code: %d\n", req_msg.func_code);
        switch (req_msg.func_code)
        {
        case FUNC_userspace_liteapi_get_node_id:
            rsp_msg.rval.int_rsp = router_nid;
            break;
        case FUNC_userspace_liteapi_join:
            LOG_INFO("  eth_port: %d\n", req_msg.msg_body.join_req.eth_port);
            LOG_INFO("  ib_port: %d\n", req_msg.msg_body.join_req.ib_port);
            LOG_INFO("  input_str: %s\n", req_msg.msg_body.join_req.input_str);
            rsp_msg.rval.int_rsp = router_nid;
            break;
        case FUNC_userspace_liteapi_alloc_remote_mem:
            LOG_INFO("  node_id: %d\n", req_msg.msg_body.alloc_remote_mem_req.node_id);
            LOG_INFO("  size: %d\n", req_msg.msg_body.alloc_remote_mem_req.size);
            LOG_INFO("  atomic_flag: %d\n", req_msg.msg_body.alloc_remote_mem_req.atomic_flag);
            LOG_INFO("  password: %d\n", req_msg.msg_body.alloc_remote_mem_req.password);
            rsp_msg.rval.int_rsp = userspace_liteapi_alloc_remote_mem(req_msg.msg_body.alloc_remote_mem_req.node_id,
                                                                      req_msg.msg_body.alloc_remote_mem_req.size,
                                                                      req_msg.msg_body.alloc_remote_mem_req.atomic_flag,
                                                                      req_msg.msg_body.alloc_remote_mem_req.password);
            break;
        case FUNC_userspace_liteapi_rdma_write:
            LOG_INFO("  lite_handler: %d\n", req_msg.msg_body.rdma_write_req.lite_handler);
            LOG_INFO("  local_addr: %p\n", req_msg.msg_body.rdma_write_req.local_addr);
            LOG_INFO("  size: %d\n", req_msg.msg_body.rdma_write_req.size);
            LOG_INFO("  offset: %d\n", req_msg.msg_body.rdma_write_req.offset);
            LOG_INFO("  password: %d\n", req_msg.msg_body.rdma_write_req.password);
            rsp_msg.rval.int_rsp = userspace_liteapi_rdma_write(req_msg.msg_body.rdma_write_req.lite_handler,
                                                                req_msg.msg_body.rdma_write_req.local_addr,
                                                                req_msg.msg_body.rdma_write_req.size,
                                                                req_msg.msg_body.rdma_write_req.offset,
                                                                req_msg.msg_body.rdma_write_req.password);
            break;
        case FUNC_userspace_liteapi_rdma_read:
            LOG_INFO("  lite_handler: %d\n", req_msg.msg_body.rdma_read_req.lite_handler);
            LOG_INFO("  local_addr: %p\n", req_msg.msg_body.rdma_read_req.local_addr);
            LOG_INFO("  size: %d\n", req_msg.msg_body.rdma_read_req.size);
            LOG_INFO("  offset: %d\n", req_msg.msg_body.rdma_read_req.offset);
            LOG_INFO("  password: %d\n", req_msg.msg_body.rdma_read_req.password);
            rsp_msg.rval.int_rsp = userspace_liteapi_rdma_read(req_msg.msg_body.rdma_read_req.lite_handler,
                                                               req_msg.msg_body.rdma_read_req.local_addr,
                                                               req_msg.msg_body.rdma_read_req.size,
                                                               req_msg.msg_body.rdma_read_req.offset,
                                                               req_msg.msg_body.rdma_read_req.password);
            break;
        case FUNC_userspace_liteapi_send_reply_imm_fast:
            LOG_INFO("  target_node: %d\n", req_msg.msg_body.send_reply_imm_fast_req.target_node);
            LOG_INFO("  port: %d\n", req_msg.msg_body.send_reply_imm_fast_req.port);
            LOG_INFO("  addr: %p\n", req_msg.msg_body.send_reply_imm_fast_req.addr);
            LOG_INFO("  size: %d\n", req_msg.msg_body.send_reply_imm_fast_req.size);
            LOG_INFO("  ret_addr: %p\n", req_msg.msg_body.send_reply_imm_fast_req.ret_addr);
            LOG_INFO("  ret_length: %p\n", req_msg.msg_body.send_reply_imm_fast_req.ret_length);
            LOG_INFO("  max_ret_size: %d\n", req_msg.msg_body.send_reply_imm_fast_req.max_ret_size);
            rsp_msg.rval.int_rsp = userspace_liteapi_send_reply_imm_fast(req_msg.msg_body.send_reply_imm_fast_req.target_node,
                                                                         req_msg.msg_body.send_reply_imm_fast_req.port,
                                                                         req_msg.msg_body.send_reply_imm_fast_req.ret_addr,
                                                                         req_msg.msg_body.send_reply_imm_fast_req.size,
                                                                         req_msg.msg_body.send_reply_imm_fast_req.ret_addr,
                                                                         req_msg.msg_body.send_reply_imm_fast_req.ret_length,
                                                                         req_msg.msg_body.send_reply_imm_fast_req.max_ret_size);
            break;
        case FUNC_userspace_liteapi_receive_message_fast:
            LOG_INFO("  port: %d\n", req_msg.msg_body.receive_message_fast_req.port);
            LOG_INFO("  ret_addr: %p\n", req_msg.msg_body.receive_message_fast_req.ret_addr);
            LOG_INFO("  receive_size: %d\n", req_msg.msg_body.receive_message_fast_req.receive_size);
            LOG_INFO("  block_call: %d\n", req_msg.msg_body.receive_message_fast_req.block_call);
            rsp_msg.rval.int_rsp = userspace_liteapi_receive_message_fast(req_msg.msg_body.receive_message_fast_req.port,
                                                                          req_msg.msg_body.receive_message_fast_req.ret_addr,
                                                                          req_msg.msg_body.receive_message_fast_req.receive_size,
                                                                          &rsp_msg.msg_body.receive_message_fast_rsp.descriptor,
                                                                          &rsp_msg.msg_body.receive_message_fast_rsp.ret_length,
                                                                          req_msg.msg_body.receive_message_fast_req.block_call);
            break;
        case FUNC_userspace_liteapi_reply_message:
            LOG_INFO("  port: %p\n", req_msg.msg_body.reply_message_req.addr);
            LOG_INFO("  ret_addr: %d\n", req_msg.msg_body.reply_message_req.size);
            LOG_INFO("  receive_size: %lu\n", req_msg.msg_body.reply_message_req.descriptor);
            rsp_msg.rval.int_rsp = userspace_liteapi_reply_message(req_msg.msg_body.reply_message_req.addr,
                                                                   req_msg.msg_body.reply_message_req.size,
                                                                   req_msg.msg_body.reply_message_req.descriptor);
            break;
        case FUNC_userspace_liteapi_register_application:
            LOG_INFO("  destined_port: %d\n", req_msg.msg_body.register_application_req.destined_port);
            LOG_INFO("  max_size_per_message: %d\n", req_msg.msg_body.register_application_req.max_size_per_message);
            LOG_INFO("  max_user_per_node: %d\n", req_msg.msg_body.register_application_req.max_user_per_node);
            LOG_INFO("  name: %s\n", req_msg.msg_body.register_application_req.name);
            LOG_INFO("  name_len: %lu\n", req_msg.msg_body.register_application_req.name_len);
            rsp_msg.rval.int_rsp = userspace_liteapi_register_application(req_msg.msg_body.register_application_req.destined_port,
                                                                          req_msg.msg_body.register_application_req.max_size_per_message,
                                                                          req_msg.msg_body.register_application_req.max_user_per_node,
                                                                          req_msg.msg_body.register_application_req.name,
                                                                          req_msg.msg_body.register_application_req.name_len);
            break;
        case FUNC_userspace_liteapi_dist_barrier:
            LOG_INFO("  num: %d\n", req_msg.msg_body.dist_barrier_req.num);
            rsp_msg.rval.int_rsp = userspace_liteapi_dist_barrier(req_msg.msg_body.dist_barrier_req.num);
            break;
        case FUNC_userspace_liteapi_query_port:
            LOG_INFO("  target_node: %d\n", req_msg.msg_body.query_port_req.target_node);
            LOG_INFO("  designed_port: %d\n", req_msg.msg_body.query_port_req.designed_port);
            rsp_msg.rval.int_rsp = userspace_liteapi_query_port(req_msg.msg_body.query_port_req.target_node,
                                                                req_msg.msg_body.query_port_req.designed_port);
            break;
        case FUNC_userspace_liteapi_alloc_local_mem:
            LOG_INFO("  name: %s\n", req_msg.msg_body.alloc_local_mem_req.name);
            LOG_INFO("  size: %lu\n", req_msg.msg_body.alloc_local_mem_req.size);
            rsp_msg.rval.int_rsp = alloc_shm_mem(req_msg.msg_body.alloc_local_mem_req.name,
                                                 req_msg.msg_body.alloc_local_mem_req.size,
                                                 &rsp_msg.msg_body.alloc_local_mem_rsp.remote_addr);
            break;
        case FUNC_userspace_liteapi_free_local_mem:
            LOG_INFO("  name: %s\n", req_msg.msg_body.free_local_mem_req.name);
            LOG_INFO("  size: %lu\n", req_msg.msg_body.free_local_mem_req.size);
            LOG_INFO("  remote_addr: %p\n", req_msg.msg_body.free_local_mem_req.remote_addr);
            rsp_msg.rval.int_rsp = free_shm_mem(req_msg.msg_body.free_local_mem_req.name,
                                                req_msg.msg_body.free_local_mem_req.size,
                                                req_msg.msg_body.free_local_mem_req.remote_addr);
            break;
        default:
            LOG_ERROR("UNKNOWN FUNC CODE\n");
            close(client_sock);
            pthread_exit(NULL);
        }

        /* Send data back to the connected socket */
        LOG_INFO("Sending data...\n");
        if (send(client_sock, &rsp_msg, sizeof(struct rpc_rsp_msg), 0) < 0)
        {
            LOG_PERROR("SEND ERROR");
            close(client_sock);
            pthread_exit(NULL);
        }
        LOG_INFO("Data sent!\n");
    }

    /* Close the sockets and exit */
    close(client_sock);
    LOG_DEBUG("Socket closed\n");
    pthread_exit(NULL);
}

int main(void)
{
    signal(SIGINT, INThandler); // handle SIGINT

    /* Connect cluster manager */
    router_nid = userspace_liteapi_join("172.16.0.254", 18500, 1);
    if (router_nid <= 0 || router_nid > 10)
    {
        LOG_ERROR("Join cluster failed. Node ID %d.\n", router_nid);
        exit(EXIT_FAILURE);
    }
    else
    {
        LOG_NORMAL("Joined cluster. Node ID %d\n", router_nid);
    }

    /* Connect virtual cluster manager */
    if (vcm_conn("172.16.0.254") < 0)
    {
        LOG_ERROR("Connect virtual cluster manager failed.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        LOG_NORMAL("Connected virtual cluster manager.\n");
    }

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
        LOG_PERROR("SOCKET ERROR");
        exit(EXIT_FAILURE);
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
        LOG_PERROR("BIND ERROR");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    /* Listen for any client sockets */
    if (listen(server_sock, backlog) < 0)
    {
        LOG_PERROR("LISTEN ERROR");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    LOG_NORMAL("socket listening...\n");

    pthread_t pth;
    fd_set readfds;
    struct timeval timeout;
    while (keepRunning)
    {
        FD_ZERO(&readfds);
        FD_SET(server_sock, &readfds);
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        int active_fds = select(server_sock + 1, &readfds, NULL, NULL, &timeout);
        if (active_fds < 0)
        {
            LOG_PERROR("SELECT ERROR");
            close(server_sock);
            exit(EXIT_FAILURE);
        }
        else if (active_fds == 0) // timeout
        {
            LOG_INFO("SELECT TIMEOUT\n");
            continue;
        }
        else if (active_fds != 1) // never be here
        {
            LOG_ERROR("Mysterious error. Active fds should be 1 but %d\n", active_fds);
            close(server_sock);
            exit(EXIT_FAILURE);
        }
        /* Accept an incoming connection */
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_sockaddr, &len)) < 0)
        {
            LOG_PERROR("ACCEPT ERROR");
            close(server_sock);
            close(client_sock);
            exit(EXIT_FAILURE);
        }
        if (pthread_create(&pth, NULL, server_accept_request, &client_sock) < 0)
        {
            LOG_PERROR("THREAD CREATE ERROR");
            close(server_sock);
            close(client_sock);
            exit(EXIT_FAILURE);
        }
    }

    /* Close the sockets and exit */
    vcm_disconn();
    close(server_sock);
    close(client_sock);
    LOG_NORMAL("Exit main\n");
    // TODO: build a thread pool and thread_join here
    // TODO: build a talbe to store address alloced by client
    //       and free them when exit

    return EXIT_SUCCESS;
}