#include "routing.hpp"

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>
#include <unordered_map>
#include <mutex>

#include "../../inc/log.h"
#include "../../inc/vcm_types.hpp"
#include "../config.hpp"

/* Reused socket fd */

static int client_sock = -1;

/**
 * Connect vcm.
 * Input:
 *    vcm_ip: server ip.
 * Return: error code.
 */
int vcm_conn(std::string vcm_ip)
{
    int len;
    struct sockaddr_in server_sockaddr;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_in));

    if ((client_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        LOG_PERROR("SOCKET ERROR");
        return -1;
    }

    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = VCM_PORT;
    len = sizeof(server_sockaddr);

    if (inet_pton(AF_INET, vcm_ip.c_str(), &server_sockaddr.sin_addr) <= 0)
    {
        LOG_PERROR("Invalid address or Address not supported");
        return -1;
    }

    if (connect(client_sock, (struct sockaddr *)&server_sockaddr, len) < 0)
    {
        LOG_PERROR("CONNECT ERROR");
        vcm_disconn();
        return -1;
    }

    return client_sock;
}

/**
 * Disconnect vcm
 */
void vcm_disconn()
{
    close(client_sock);
    client_sock = -1;
}

/**
 * Handle rpc
 * Input:
 *    msg: request message structure
 * Output:
 *    msg: response message structure
 * Return: error code
 */
int vcm_handler(struct vcm_msg *msg)
{
    if (client_sock == -1) // Not connected yet
        return -1;         // But Nerver here

    if (send(client_sock, msg, sizeof(struct vcm_msg), 0) < 0)
    {
        LOG_PERROR("SEND ERROR");
        vcm_disconn();
        return -1;
    }
    LOG_INFO("Data sent!\n");

    /* Read the data sent from the server and print it. */
    if (recv(client_sock, msg, sizeof(struct vcm_msg), 0) < 0)
    {
        LOG_PERROR("RECV ERROR");
        vcm_disconn();
        return -1;
    }
    LOG_INFO("DATA RECEIVED\n");

    return 0;
}

static std::mutex map_mutex;
static std::unordered_map<int, int> vid_nid_map_cache;
static std::unordered_map<std::string, int> cid_vid_map;

/* PUBLIC FUNCTIONS */

int vcm_get_node_id(std::string cid)
{
    std::lock_guard<std::mutex> lock(map_mutex);

    std::unordered_map<std::string, int>::const_iterator got = cid_vid_map.find(cid);
    if (got == cid_vid_map.end())
        return -1;
    else
        return got->second;
}

int vcm_join(std::string cid)
{
    std::lock_guard<std::mutex> lock(map_mutex);

    std::unordered_map<std::string, int>::const_iterator got = cid_vid_map.find(cid);
    if (got == cid_vid_map.end())
    {
        struct vcm_msg msg
        {
            .func_code = NID_VID,
            .id = router_nid
        };
        if (vcm_handler(&msg) < 0)
            return -1;
        else
        {
            int vid = msg.id;
            cid_vid_map.insert({cid, vid});
            vid_nid_map_cache.insert({vid, router_nid});
            return vid;
        }
    }
    else
    {
        return got->second;
    }
}

int vcm_vid_nid(int vid)
{
    std::lock_guard<std::mutex> lock(map_mutex);

    std::unordered_map<int, int>::const_iterator got = vid_nid_map_cache.find(vid);
    if (got == vid_nid_map_cache.end())
    {
        struct vcm_msg msg
        {
            .func_code = VID_NID,
            .id = vid
        };
        if (vcm_handler(&msg) < 0)
            return -1;
        else
        {
            vid_nid_map_cache.insert({vid, msg.id});
            return msg.id;
        }
    }
    else
    {
        return got->second;
    }
}