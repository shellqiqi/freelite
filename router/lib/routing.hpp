#ifndef VCM_REQ_H
#define VCM_REQ_H

#include <string>
#include "../../inc/vcm_types.hpp"

int vcm_conn(std::string vcm_ip);
void vcm_disconn();

int vcm_handler(struct vcm_msg *msg);

int vcm_get_node_id(std::string cid);
int vcm_join(std::string cid);
int vcm_vid_nid(int vid);

#endif