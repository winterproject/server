#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 9000
#define MAX 512

char quit[] = "exit";
int numClient =0;
int clientSock[MAX];

int getMaxfd(int);

int main()
{
	int connSock, listenSock;
	struct sockaddr_in s_addr, c_addr;
	int len, i , n, j;
	char rcvBuffer[BUFSIZ];
	int maxfd;

	fd_set read_fds;
	
	listenSock = socket(PF_INET, SOCK_STREAM, 0 );
	
	memset(&s_addr,0,sizeof(s_addr));
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	if(bind(listenSock, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1 )
	{
		printf("can not bind\n");
		return -1;
	}

	if( listen(listenSock, 5) == -1 )
	{
		printf("listen fail\n");
		return -1;
	}

	while(1)
	{
		maxfd = getMaxfd(listenSock) + 1;

		FD_ZERO(&read_fds);
		FD_SET(listenSock, &read_fds);
	
	
		for(i = 0; i< numClient; i++)
		{
			FD_SET(clientSock[i], &read_fds);
		}
			
		if(select(maxfd, &read_fds, NULL, NULL, NULL) < 0 )
		{
			printf("select error\n");
			exit(-1);
		}

		printf("waiting---\n");

		if( FD_ISSET(listenSock, &read_fds))
		{
			connSock = accept(listenSock, (struct sockaddr *) &c_addr, &len);
			clientSock[numClient++] = connSock;
			printf("dd");
		}


		for( i=0; i< numClient; i++)
		{
			if(FD_ISSET(clientSock[i], &read_fds))
			{
				if( (n = read(clientSock[i] , rcvBuffer, sizeof(rcvBuffer))) != 0 )
				{
				/*	it dosen't work	
					if ( rcvBuffer[0] == 'p' && rcvBuffer[1] == '1')
					{
						char buff[BUFSIZ];
						FILE *fp;
				
						fp = popen("wakeonlan -i 59.7.14.242 D0:50:99:1D:47:AE","r");
						if( NULL == fp)
						{
							perror("  wol fail\n");
							
						}
						
						while(fgets(buff, BUFSIZ,fp) )
							printf("%s", buff);

						pclose(fp);
					}*/    
					if ( strncmp(rcvBuffer, quit, 4) == 0)
					{
						close(clientSock[i]);
						if( i != numClient-1)
							clientSock[i] = clientSock[numClient-1];
						numClient--;
						continue;
					}
					else
					{
						for( j=0; j < numClient; j++)
						{
							write(clientSock[j], rcvBuffer, sizeof(rcvBuffer));
							printf("%s",rcvBuffer);
						}	
					}
				}
				else
				{
					char cw[] = "wait";
					printf("qq\n");
					continue;
				}
			}
		}	
	}
}

int getMaxfd(int n)
{
	int max = n;
	int i;
	
	for(i =0 ; i<numClient; i++)
	{
		if(clientSock[i] > max )
			max = clientSock[i];
	}

	return max;
}
