# 与 libuv 集成设计
- uv_run 是一个指定的全局线程，每次发起请求时全局判断 uv_run 线程是否启动，如果没有启动则注册事件后启动，如果已经启动则通过 uv_async 的方式注册一个新的事件。
- 支持 chunk 方式持续写入 http ，在 chunk 模式下，每个 http_handle 维护一个 uv_async
- 每次请求对应一个 token，通过 token 判断 请求是否完成。
- 