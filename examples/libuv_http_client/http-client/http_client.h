#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (http_resp_h)(int err, const struct http_msg *msg, void *arg);
typedef int  (http_data_h)(const uint8_t *buf, size_t size,
               const struct http_msg *msg, void *arg);


typedef struct http_handle_s {

} http_handle_t;


http_create(http_handle_t **http_handle, http_resp_h *resph, http_data_h *datah);

void http_set_header(http_handle_t *http_handle, const char *a_hdr, const char *a_val);
int http_set_body(http_handle_t *http_handle, const char *a_body, int a_len);
void http_set_timeout(http_handle_t *http_handle, int timeout_second);
void http_set_uri(http_handle_t *http_handle, const char *method, const char *uri);
void http_set_is_chunk(http_handle_t *http_handle, bool is_chunk);
void http_set_buffer_size(http_handle_t *http_handle, size_t buffer_size);

http_request(http_handle_t *http_handle);
http_chunk_write(http_handle_t *http_handle, char *buf, size_t buf_len, bool is_end);

http_destroy(http_handle_t *http_handle);


#ifdef __cplusplus
}
#endif

#endif /* HTTP_CLIENT_H */
