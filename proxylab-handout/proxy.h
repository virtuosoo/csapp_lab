/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000  //~= 1MB
#define MAX_OBJECT_SIZE 102400  //100 KB
#define SEM_QUEUE_SIZE 16
#define THREAD_NUM 16

#define HOSTNAME_LEN 256
#define PORT_LEN 8
#define METHOD_LEN 16
#define HOST_LEN 128
#define HTTP_VERSION_LEN 16

#define ARRAY_LEN(arr) sizeof(arr) / sizeof((arr)[0])