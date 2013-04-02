#include "CCESocket.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#pragma comment(lib,"WS2_32.lib")

class Win32SocketInitClass  
{  
public:  
	Win32SocketInitClass()  
    {  
		//≥ı ºªØ WS2_32.lib  
		WSADATA   wsa;  
		WSAStartup(MAKEWORD(2,0),&wsa);        
	}  
    ~Win32SocketInitClass(void)  
	{  
		WSACleanup();  
    }  
};
Win32SocketInitClass initObject;

#else
#include <signal.h>
#endif

USING_NS_CC;


// CCESocket
CCESocket::CCESocket()
	: m_sockfd(0)
	, m_fakefd_w(0)
	, m_fakefd_r(0)
{
}

CCESocket::~CCESocket()
{	
	clear();
}

void CCESocket::createFakeFD()
{
	if(m_fakefd_w) {
		return;
	}
	int tmpfd,tmpfdw,tmpfdr;
	struct hostent * he;  
	struct sockaddr_in server;
	struct sockaddr_in* lisinfo;
	if ((he = gethostbyname("127.0.0.1")) == NULL) {
		return;
	}  
	
	if (( tmpfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		return;
	}
  
    memset(&server, 0x00, sizeof(server));  
  
    server.sin_family = AF_INET;  
    server.sin_port = 0;  
    server.sin_addr = *((struct in_addr *) he->h_addr);  
	
	if(bind(tmpfd, (struct sockaddr*) &server, sizeof(server))<0) {
		closesocket(tmpfd);
		return;
	}

	struct sockaddr addr;    /* generic socket name */
	int addrlen = sizeof(addr); /* need int for return value */
	if ( getsockname( tmpfd, &addr, &addrlen )< 0 ) {
		closesocket(tmpfd);
		return;
	}
	lisinfo = (struct sockaddr_in*) &addr;
	if ( listen ( tmpfd, 2 ) < 0 ) {
		closesocket(tmpfd);
		return;
	}

	if((tmpfdw = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		closesocket(tmpfd);
		return;
	}
	int err;
	// u_long iMode = 1;
	// ioctlsocket(tmpfdw, FIONBIO, &iMode);
	if ( ( connect ( tmpfdw,  &addr, addrlen ) ) < 0 ) {
		err = errno;
		closesocket(tmpfdw);
		closesocket(tmpfd);		
		return;
	}

	if((tmpfdr = accept(tmpfd,0,0))<0) {
		closesocket(tmpfdw);
		closesocket(tmpfd);		
		return;
	}
	closesocket(tmpfd);
	m_fakefd_w = tmpfdw;
	m_fakefd_r = tmpfdr;
}

bool CCESocket::open(const char* host,int port)
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	// linux must:
	signal(SIGPIPE, SIG_IGN);
#endif

	CC_ASSERT(m_sockfd==0);
	struct hostent * he;  
    struct sockaddr_in server;  
  
    if ((he = gethostbyname(host)) == NULL) {  
        CCLOG("gethostbyname fail, %d, %s", errno, host);  
        return false;
    }  
  
    if (( m_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		CCLOG("network client socket failure %d/n", errno);
		return false;
	}
  
    memset(&server, 0x00, sizeof(server));  
  
    server.sin_family = AF_INET;  
    server.sin_port = htons(port);  
    server.sin_addr = *((struct in_addr *) he->h_addr);  
	
	CCLOG("connect %s : %d ...",host, port);
	if ( ( connect ( m_sockfd,  (struct sockaddr*) &server, sizeof(server) ) ) < 0 ) {
		CCLOG("network client connect failure %d", errno);
        close ();
		return false;
	}
	return true;
}

bool CCESocket::write(const char* buf,int len)
{
	if(!isOpen())return false;
	int w = send(m_sockfd, buf, len, 0);
	if(w <0) {
		CCLOG("socket send fail %d", errno);
		close();
		return false;
	}
	return true;
}

int CCESocket::read(char* buf,int len,int time)
{
	if(!isOpen())return -1;
	if(time>=0) {
		int maxfd = m_fakefd_r!=NULL?max(m_fakefd_r,m_sockfd):m_sockfd;
		fd_set fdR;
		struct  timeval tm;
		tm.tv_sec = time/1000;
		tm.tv_usec = time%1000*1000;
		FD_ZERO(&fdR);
		FD_SET(m_sockfd, &fdR);
		if(m_fakefd_r!=NULL) {
			FD_SET(m_fakefd_r, &fdR);
		}
		int ret = select(maxfd+1,&fdR,NULL,NULL,&tm);
		if(ret<0) {
			CCLOG("select fail, %d", errno);
			close();
			return -1;
		}
		if(ret==0) {
			//timeout
			return 0;
		}		
		if(m_fakefd_r!=NULL && FD_ISSET(m_fakefd_r,&fdR)) {
			char tmpbuf[10];
			if(recv(m_fakefd_r,tmpbuf,10,0)<0) {
				closesocket(m_fakefd_r);
				closesocket(m_fakefd_w);
				m_fakefd_r = NULL;
				m_fakefd_w = NULL;
			}
		}
		if(!FD_ISSET(m_sockfd, &fdR)) {
			return 0;
		}
	}
	memset(buf, 0, len);  
	int r = recv(m_sockfd, buf, len, 0);  
	if (r <= 0)
	{
		//client close  
		CCLOG("remote offline");
		close();
		return -1;
	}
	return r;
}

void CCESocket::close()
{
	if(m_sockfd) {
		closesocket(m_sockfd);
		m_sockfd = 0;
	}
}

void CCESocket::clear()
{
	close();	
	if(m_fakefd_r) {
		closesocket(m_fakefd_r);
		m_fakefd_r = 0;
	}
	if(m_fakefd_w) {
		closesocket(m_fakefd_w);
		m_fakefd_w = 0;
	}
}

void CCESocket::wakeup()
{
	if(m_fakefd_w) {
		char fakebuf[1];   
		fakebuf[0] = 1;
		::send(m_fakefd_w, fakebuf, 1, 0);
	}
}