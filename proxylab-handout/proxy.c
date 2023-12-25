#include "csapp.h"

#define DEBUG 0

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000  //~= 1MB
#define MAX_OBJECT_SIZE 102400  //100 KB

#define HOSTNAME_LEN 256
#define PORT_LEN 8
#define METHOD_LEN 16
#define HOST_LEN 128
#define HTTP_VERSION_LEN 16

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *conn_hdr = "Connection: close\r\n";
static const char *proxy_conn_hdr = "Proxy-Connection: close\r\n";
static const char *httpPrefix = "http://";
static const int httpPrefixLen = 7;

static char *defaulWebPort = "80";
static char *proxyPort = "8082";
static char *version = "HTTP/1.0";


char *supplied_hdrs[] = {"User-Agent", "Connection", "Proxy-Connection", "Host"};

void *work(void *connfd);
int parseRequestLine(char *requstLine, char *method, char *host, char **uri);
int connectToHost(char *host);
int rio_readlineb_limit(rio_t *rp, void *usrbuf, size_t maxlen);

int main()
{
    int listenfd, connfd;
    char host[HOSTNAME_LEN], port[PORT_LEN];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    listenfd = Open_listenfd(proxyPort);

    while (1) {
        pthread_t tid;
        clientlen = sizeof(clientaddr);
        int *connfd = (int *) Malloc(sizeof(int));
        *connfd = accept(listenfd, (SA*) &clientaddr, &clientlen);
        if (*connfd < 0) {
            printf("accept failed, %s\n", strerror(errno));
        }
        Pthread_create(&tid, NULL, work, (void *) connfd);
    }
    
    return 0;
}

void* work(void *fd)
{
    int connfd = *((int *) fd), rc;
    Free(fd);
    pthread_detach(pthread_self());
    char method[METHOD_LEN], host[HOST_LEN];
    char buf[MAXLINE];

    rio_t rio;
    rio_readinitb(&rio, connfd);
    rc = rio_readlineb_limit(&rio, buf, MAXLINE); //read requst line
    if (rc < 0) {
        close(connfd);
        return NULL;
    }

    #if DEBUG > 0
    printf("request line %s");
    #endif

    char *uri;
    if (parseRequestLine(buf, method, host, &uri) < 0) {
        printf("parse request line failed, %s\n", buf);
    }

    int clientfd = connectToHost(host);

}

//must read a '\n' within length maxlen, otherwise return -1, means error
int rio_readlineb_limit(rio_t *rp, void *usrbuf, size_t maxlen)
{
    int rc = rio_readlineb(rp, usrbuf, maxlen);
    if (rc < 0) {
        printf("read failed, %s\n", strerror(errno));
        return -1;
    }

    if (((char *)usrbuf)[rc - 1] != '\n') { //line is too long
        printf("line is too long, %s\n", usrbuf);
        return -1;
    }
    return rc;
}

/*
    in:requstLine
    out:method, host, uriAndVersion

    requestLine is like 
    "GET http://www.cmu.edu/ HTTP/1.1"
    "GET http://www.cmu.edu/hub/index.html HTTP/1.1"
    method uri                             version
*/
int parseRequestLine(char *requstLine, char *method, char *host, char **uri)
{
    char *c = strchr(requstLine, ' '), *url = c + 1; //find the first space
    int len = c - requstLine;               //the method length
    memcpy(method, requstLine, len);
    method[len] = '\0';

    if (strncmp(url, httpPrefix, httpPrefixLen) != 0) { //uri must starts with "http://"
        return -1;
    }
    
    url = url + httpPrefixLen; 
    c = strchr(url, ' ');   //find the space before version part
    *c = '\0';              //we don not need the version part and this way we don not need to copy the uri part
    *uri = strchr(url, '/');

    len = *uri - url;       //the host length
    memcpy(host, url, len);
    host[len] = '\0';

    #if DEBUG > 0
    printf("parse requst line success. method(%s), host(%s), uri(%s)\n", method, host, uri);
    #endif

    return 0;
}

int connectToHost(char *host)
{
    char *domainName = host, *port;
    char *c = strchr(host, ':');
    if (c == NULL) {
        port = defaulWebPort;
    } else {
        *c = '\0';
        port = c + 1;
    }


}