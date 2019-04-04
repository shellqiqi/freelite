#include "lib/lite-lib.h"

int main(int argc, char *argv[])
{
    printf("userspace_liteapi_join: %d\n",
           userspace_liteapi_join("172.16.0.254", 18500, 1));
    printf("userspace_liteapi_get_node_id: %d\n",
           userspace_liteapi_get_node_id());
    return 0;
}
