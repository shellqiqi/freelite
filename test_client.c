#include "lite-lib.h"

int main(int argc, char *argv[])
{
    printf("Ready to join a LITE cluster\n");
    userspace_liteapi_join("172.16.0.254", 18500, 1);
    printf("after join cluster as %d\n", userspace_liteapi_get_node_id());
    return 0;
}
