#include <stdio.h>
#include <string.h>
#include "http-client/http_client.h"
#include <gnu/libc-version.h>


int main(int argc, char *argv[])
{
    char *url;
    char data[1024], response[4096];
    int  i, ret, size;

    HTTP_INFO hi1, hi2;


    // Init http session. verify: check the server CA cert.
    http_init(&hi1, true);

    // Test a http get method.
    url = "https://kodo.router7.com/index.html";
    ret = http_get(&hi1, url, response, sizeof(response));
    printf("return code: %d \n", ret);
    printf("return body: %s \n", response);




    // Test a https post with the chunked-encoding data.
    url = "https://httpbin.org/post";
    if(http_open_chunked(&hi2, url) == 0)
    {
        size = sprintf(data, "[{\"message\":\"Hello, https_client %d\"},", 0);
        if(http_write_chunked(&hi2, data, size) != size)
        {
            http_strerror(data, 1024);
            printf("socket error: %s \n", data);
            goto error;
        }
        for(i=1; i<4; i++)
        {
            size = sprintf(data, "{\"message\":\"Hello, https_client %d\"},", i);
            if(http_write_chunked(&hi2, data, size) != size)
            {
                http_strerror(data, 1024);
                printf("socket error: %s \n", data);
                goto error;
            }
        }
        size = sprintf(data, "{\"message\":\"Hello, https_client %d\"}]", i);
        if(http_write_chunked(&hi2, data, strlen(data)) != size)
        {
            http_strerror(data, 1024);
            printf("socket error: %s \n", data);
            goto error;
        }
        ret = http_read_chunked(&hi2, response, sizeof(response));
        printf("return code: %d \n", ret);
        printf("return body: %s \n", response);
    }
    else
    {
        http_strerror(data, 1024);
        printf("socket error: %s \n", data);
    }
    error:


error:

    http_close(&hi1);
    http_close(&hi2);

    return 0;
}
