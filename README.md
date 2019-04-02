# Freelite

## 参数传递问题

以下三个函数的参数意义不明确，特别是第二个函数参数中带有`uintptr_t *descriptor`，第三个中有`uintptr_t descriptor`

```
userspace_liteapi_send_reply_imm_fast
userspace_liteapi_receive_message_fast
userspace_liteapi_reply_message
```

在`lite_core.c`中`userspace_liteapi_send_reply_imm_fast`有强制类型转换

```c
struct imm_message_metadata *descriptor;
*reply_descriptor = (uintptr_t) descriptor;
```

需要进一步确定这三个函数的变量是否是输出型
