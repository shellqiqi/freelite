#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include <unordered_map>
#include <mutex>

#include "../inc/log.h"
#include "../inc/vcm_types.hpp"

static std::mutex vcm_mutex;

static std::unordered_map<int, int> vid_nid_map;
static int vid = 0;

static bool keepRunning = true;

void INThandler(int sig)
{
    keepRunning = false;
}

int get_new_vid(int nid)
{
    std::lock_guard<std::mutex> lock(vcm_mutex);

    ++vid;
    vid_nid_map.insert({vid, nid});
    return vid;
}

int get_nid_by_vid(int vid)
{
    std::lock_guard<std::mutex> lock(vcm_mutex);

    std::unordered_map<int, int>::const_iterator got = vid_nid_map.find(vid);
    if (got == vid_nid_map.end())
        return -1;
    else
        return got->second;
}

void *server_accept_request(void *fd)
{
    socklen_t len;
    int client_sock = *((int *)fd);
    struct vcm_msg msg;
    struct sockaddr_in client_sockaddr;
    memset(&msg, 0, sizeof(struct vcm_msg));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_in));

    /* Get the name of the connected socket */
    len = sizeof(client_sockaddr);
    if (getpeername(client_sock, (struct sockaddr *)&client_sockaddr, &len) < 0)
    {
        LOG_PERROR("GETPEERNAME ERROR");
        close(client_sock);
        pthread_exit(NULL);
    }
    LOG_DEBUG("Client addr: %s\n", inet_ntoa(client_sockaddr.sin_addr));

    while (1)
    {
        /* Read and print the data incoming on the connected socket */
        LOG_INFO("waiting to read...\n");
        int ret_len = recv(client_sock, &msg, sizeof(struct vcm_msg), 0);
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
        LOG_INFO("  func_code: %d\n", msg.func_code);
        switch (msg.func_code)
        {
        case VID_NID:
            get_nid_by_vid(msg.id);
            break;
        case NID_VID:
            get_new_vid(msg.id);
            break;
        default:
            LOG_ERROR("UNKNOWN FUNC CODE\n");
            close(client_sock);
            pthread_exit(NULL);
        }

        /* Send data back to the connected socket */
        LOG_INFO("Sending data...\n");
        if (send(client_sock, &msg, sizeof(struct vcm_msg), 0) < 0)
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

    int server_sock, client_sock;
    socklen_t len;
    struct sockaddr_in server_sockaddr;
    struct sockaddr_in client_sockaddr;
    int backlog = 10;
    int port = 11451;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_in));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_in));

    if ((server_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG_PERROR("SOCKET ERROR");
        exit(EXIT_FAILURE);
    }

    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_addr.s_addr = INADDR_ANY;
    server_sockaddr.sin_port = port;
    len = sizeof(server_sockaddr);

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
    close(server_sock);
    close(client_sock);
    LOG_NORMAL("Exit main\n");
    // TODO: build a thread pool and thread_join here

    return EXIT_SUCCESS;
}