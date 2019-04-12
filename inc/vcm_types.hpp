#ifndef VCM_TYPES_H
#define VCM_TYPES_H

#define VID_NID 0
#define NID_VID 1

struct vcm_msg
{
    int func_code;
    int id;
};

#endif