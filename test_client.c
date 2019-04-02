#include "lib/lite-lib.h"

int main(int argc, char *argv[])
{
    printf("Ready to join a LITE cluster\n");
    int rval = userspace_liteapi_join("127.0.0.1", 18500, 1);
    printf("userspace_liteapi_join returned %d\n", rval);
    printf("after join cluster as %d\n", userspace_liteapi_get_node_id());
    return 0;
}
