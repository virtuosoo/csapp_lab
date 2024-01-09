#include "csapp.h"
#include "Queue.h"
#include "proxy.h"
#include "cache.h"

#define DEBUG 1

static const char *httpPrefix = "http://";
static const int httpPrefixLen = 7;

static char *defaulWebPort = "80";
static char *version = "HTTP/1.0";


char *supplied_hdrs[] = {"User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n", 
                        "Connection: close\r\n", 
                        "Proxy-Connection: close\r\n"};

Queue *q;
CacheList *list;

void *thread(void *arg);
void *work(int connfd);
int parseRequestLine(char *requstLine, char *method, char *host, char **uri);
int connectToHost(char *host);
int rio_readlineb_limit(rio_t *rp, void *usrbuf, size_t maxlen);
int forwardRequestHdrs(rio_t *rioClient, rio_t *rioServer);
int forwardResponse(rio_t *rioClient, rio_t *rioServer, CacheNode *cache);
int readRequstHdrs(rio_t *rioClient);

int main(int argc, char **argv)
{
    q = newSemQueueInit(SEM_QUEUE_SIZE);
    list = (CacheList *) malloc(sizeof(CacheList));
    initCacheList(list, MAX_CACHE_SIZE);
    pthread_t tids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_create(&tids[i], NULL, thread, NULL);
    }
    
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    listenfd = Open_listenfd(argv[1]);
    Signal(SIGPIPE,SIG_IGN);  
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = accept(listenfd, (SA*) &clientaddr, &clientlen);
        if (connfd < 0) {
            printf("accept failed, %s\n", strerror(errno));
        }
        q->put(q, connfd);
    }
    
    return 0;
}

void *thread(void *arg)
{
    pthread_detach(pthread_self());
    while (1) {
        int fd = q->get(q);
        work(fd);
    }
}

void* work(int fd)
{
    int clientfd = fd, rc;  //fd that communicate with client
    char method[METHOD_LEN], host[HOST_LEN];
    char buf[MAXLINE], sendBuf[MAXLINE];
    CacheNode *cache = NULL;

    rio_t rioClient, rioServer;
    rio_readinitb(&rioClient, clientfd);
    rc = rio_readlineb_limit(&rioClient, buf, MAXLINE); //read requst line
    if (rc < 0) {
        goto closeClient;
    }

    #if DEBUG > 0
    printf("request line %s", buf);
    #endif

    char *uri;
    if (parseRequestLine(buf, method, host, &uri) < 0) {
        printf("parse request line failed, %s\n", buf);
        goto closeClient;
    }
    char url[MAXLINE];
    sprintf(url, "%s%s", host, uri);
    cache = findNodeByUrl(list, url);
    if (cache != NULL) {
        if (readRequstHdrs(&rioClient) < 0) {
            goto closeClient;
        }
        rio_writen(clientfd, cache->data, cache->nodeSize);
        goto closeClient;
    } else {
        cache = newCacheNode(1);
        setNodeUrl(cache, url);
    }

    int serverfd = connectToHost(host); //fd that communicate with server
    if (serverfd < 0) {
        printf("connect to server failed\n");
        goto closeClient;
    }

    rio_readinitb(&rioServer, serverfd);

    sprintf(sendBuf, "%s %s %s\r\n", method, uri, version);
    if (rio_writen(serverfd, sendBuf, strlen(sendBuf)) < 0) {
        printf("send requst line failed, error(%s)\n", strerror(errno));
        goto closeBoth;
    }
    sprintf(sendBuf, "Host: %s\r\n", host);
    if (rio_writen(serverfd, sendBuf, strlen(sendBuf)) < 0) {
        printf("send Host header failed, error(%s)\n", strerror(errno));
        goto closeBoth;
    }

    if (forwardRequestHdrs(&rioClient, &rioServer) < 0) {
        printf("forward request headers failed, error(%s)\n", strerror(errno));
        goto closeBoth;
    }

    if (forwardResponse(&rioClient, &rioServer, cache) < 0) {
        printf("forward response failed, error(%s)\n", strerror(errno));
        goto closeBoth;
    }

    if (cache->valid) {
        insertNodeToList(list, cache);
    }

closeBoth:
    close(serverfd);
closeClient:
    close(clientfd);
    if (cache != NULL && !cache->valid) {
        deleteCacheNode(cache);
    }
    return NULL;
}

//must read a '\n' within length maxlen, otherwise return -1, means error
int rio_readlineb_limit(rio_t *rp, void *usrbuf, size_t maxlen)
{
    int rc = rio_readlineb(rp, usrbuf, maxlen);
    if (rc < 0) {
        printf("read failed, %s\n", strerror(errno));
        return -1;
    }

    if (((char *)usrbuf)[rc - 1] != '\n') { //line is too long or the other side closed the socket
        printf("read a line failed, maybe it is too long or the other side \
                has closed the socket %s\n", (char *)usrbuf);
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

    #if DEBUG > 0
    printf("parse method %s\n", method);
    #endif
    if (strncmp(url, httpPrefix, httpPrefixLen) == 0) { //uri starts with "http://"
        url = url + httpPrefixLen;     //skip throw the prefix
    }
    
    c = strchr(url, ' ');   //find the space before version part
    *c = '\0';              //we don not need the version part and this way we don not need to copy the uri part
    *uri = strchr(url, '/');
    if (*uri == NULL) {
        *uri = "/";
        len = c - url;
    } else {
        len = *uri - url;       //the host length
    }

    memcpy(host, url, len);
    host[len] = '\0';

    #if DEBUG > 0
    printf("parse requst line success. method(%s), host(%s), uri(%s)\n", method, host, *uri);
    #endif

    return 0;
}

int connectToHost(char *s)
{
    char host[HOST_LEN];
    strcpy(host, s);
    char *domainName = host, *port;
    char *c = strchr(host, ':');
    if (c == NULL) {
        port = defaulWebPort;
    } else {
        *c = '\0';
        port = c + 1;
    }

    return open_clientfd(domainName, port);
}

int forwardRequestHdrs(rio_t *rioClient, rio_t *rioServer)
{
    char buf[MAXLINE];
    while (1) {
        int rc;
        if ((rc = rio_readlineb_limit(rioClient, buf, MAXLINE)) < 0) {
            return -1;
        }

        if (strncmp(buf, "Host", 4) == 0) { //we have sent Host header before, ignore here
            continue;
        }
        
        int i;
        for (i = 0; i < ARRAY_LEN(supplied_hdrs); ++i) {
            char *p = strchr(supplied_hdrs[i], ':');
            int len = p - supplied_hdrs[i];
            if (strncmp(buf, supplied_hdrs[i], len) == 0) {
                break;
            }
        }

        int wc;
        if (i == ARRAY_LEN(supplied_hdrs)) { //send the original header
            wc = rio_writen(rioServer->rio_fd, buf, rc);
        } else {
            wc = rio_writen(rioServer->rio_fd, supplied_hdrs[i], strlen(supplied_hdrs[i]));
        }

        if (wc < 0) {
            return -1;
        }
        #if DEBUG > 0
        printf("forward request header %s", buf);
        #endif
        if (strcmp(buf, "\r\n") == 0) break;
    }
    return 1;
}

int forwardResponse(rio_t *rioClient, rio_t *rioServer, CacheNode *cache)
{
    int rc, wc;
    char buf[MAXLINE];
    while (1) {
        if ((rc = rio_readnb(rioServer, buf, MAXLINE)) < 0) {
            return -1;
        }
        appendNodeData(cache, buf, rc);
        if ((wc = rio_writen(rioClient->rio_fd, buf, rc)) < 0) {
            return -1;
        }
        
        #if DEBUG > 0
        printf("forward response %s", buf);
        #endif
        if (rc == 0) break;
    }
    return 0;
}

int readRequstHdrs(rio_t *rioClient)
{
    char buf[MAXLINE];
    while (1) {
        int rc;
        if ((rc = rio_readlineb_limit(rioClient, buf, MAXLINE)) < 0) {
            return -1;
        }

        if (strcmp(buf, "\r\n") == 0) break;
    }
    return 1;
}