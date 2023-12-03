#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <calcLib.h>
#include <netdb.h>
//#define DEBUG
#include <math.h>
//#include <complex.h>

int main(int argc, char *argv[])
{	
	//declaring the variables
	char delim[]=":";
	char *Desthost=strtok(argv[1],delim);
	char *Destport=strtok(NULL,delim); 
	int port=atoi(Destport);

	//int descriptor;
	struct addrinfo hints; 
	struct addrinfo* res=0;
	struct addrinfo *bob;
	char addrstring[100];
		
	printf("Host %s, and port %d.\n",Desthost,port);
	
	// using the address info//lecture notes and codes//patrik Arlos 
	char *ptr;
	int n;
  
	bzero(&hints, sizeof(hints));
 	hints.ai_family = 0; // AF_INET/6;
	hints.ai_socktype=SOCK_STREAM; //SOCK_STREAM;, use 0 for both type
  	hints.ai_protocol=0;
  	hints.ai_flags = AI_ALL; //AI_CANONNAME;
  	
  	  while (--argc > 0) 
  	  {
    		ptr = *++argv;   
    		if ( (n  = getaddrinfo(ptr, "domain", &hints, &res) ) != 0) 
    		{
      		fprintf(stderr,"getaddrinfo error for host: %s: %s",ptr, gai_strerror(n));
      		continue;
    		}
    	 	bob = res;
		do {
      			inet_ntop(bob->ai_family, &((struct sockaddr_in *)bob->ai_addr)->sin_addr,addrstring,100);
      			#ifdef DEBUG 
      			/*printf("IP; %s Protocol; %d \n",addrstring,bob->ai_protocol );*/
      			#endif
     			bob = bob->ai_next;
		} while (bob != NULL );
  	}    	        		
	int err=getaddrinfo(Desthost,Destport,&hints,&res);
	if (err!=0) 
		{
    		perror("failed to resolve remote socket address...");
    		exit(1);
		}
	//variable declaration
  	char myIP[16];
   	unsigned int myPort;
    	struct sockaddr_in my_addr;
	// Create the socket
	int sockdes=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	if (sockdes ==-1) 
		{
  		 perror("failed to create the socket...");
  		 exit(1);
		}
	else
		{
		// Connect to server
		if (connect(sockdes,res->ai_addr,res->ai_addrlen)==-1) 
			{
	    		perror("Connection to server failed...");
	    		exit(1);
			}
			else
			{
			/*printf("connection established...\n");*/
			}
		}	

	// Get my ip address and port
    	bzero(&my_addr, sizeof(my_addr));
    	socklen_t len = sizeof(my_addr);
	getsockname(sockdes, (struct sockaddr *)&my_addr, &len);
    	inet_ntop(res->ai_family, &my_addr.sin_addr, myIP, sizeof(myIP));
    	myPort = ntohs(my_addr.sin_port);
    	        #ifdef DEBUG 
   		printf("Connect to %s: %u Local %s : %u.\n",Desthost,port,myIP,myPort);
   		#endif
	//release the memory copied by address info
	freeaddrinfo(res); 			
	//variable declaration								
	char serverbuff[256];
	char buff1[256];
	const char* resp ="OK\n";	
		//receiving /reading the protocol type from server
		if(recv(sockdes, &serverbuff, sizeof(serverbuff), 0) ==-1) 	
			{
			perror("Protocol mismatch error...");
			exit(0);
			}
			else
			{
			#ifdef DEBUG
			//printf("%s", serverbuff);			//printing the current protocol
			#endif
			//only protocol 1.0, not neccessary to check for other protoco;
			}
			if (strcmp(serverbuff,"TEXT TCP 1.0\n\n")== 0) 	
			{
				//sending OK message to the server											
				if(send(sockdes, resp, strlen(resp), 0) ==-1)	 
					{
					perror("error sending ok message to server...");
					exit(0);
					}
				else
					{
					/*printf("OK message sent to the server\n");*/
					}
			}
			else
			{
			perror("the protocols do not match..");
			exit(0);
			}//end of comparing the protocols		
			memset(buff1,0,sizeof(buff1));	
			//read or receive the operator and values randomly created by the server 
			if(recv(sockdes, &buff1, sizeof(buff1), 0) ==-1 )
			{
			perror("did not receive operator and values from server...");
			exit(1);
			}
			else
				{						
				/*printf("%s", resp);	*/			
				printf("ASSIGNMENT: %s", buff1 );				
				//variable declaration		
						initCalcLib();
					  	double f1,f2,fresult;
					  	int i1,i2,iresult;	
						int rv=0;
						char command[10];
						//using string compare check the operator given and perform the random operation provided by the server
						// float check
						rv=sscanf(buff1,"%s %lg %lg",command,&f1,&f2);
						if(command[0]=='f')
						{
					    	if (rv == EOF ) 
					    	{
					      	perror("Sscanf failed.\n");
					      	exit(1);
					    	}
	    					if(strcmp(command,"fadd")==0)
		    				{
								fresult=f1+f2;
							} 
							else if (strcmp(command, "fsub")==0)
							{
		      					fresult=f1-f2;
		   					} 
	   						else if (strcmp(command, "fmul")==0)
	   						{
	      						fresult=f1*f2;
	   						}
	   						else if (strcmp(command, "fdiv")==0)
	   						{
								fresult=f1/f2;
							}
							else 
							{
				 		   		printf("No match\n");
				    			}
				    			//sprintf(buff1,"%8.8g\n",fresult);
				    			sprintf(buff1,"%s %8.8g %8.8g",command,&f1,&f2);
				    			memset(buff1,0,sizeof(buff1));
							/*printf("%s %8.8g %8.8g = %8.8g\n",command,f1,f2,fresult);*/ //--> checking my results
							#ifdef DEBUG 
							printf("Calculated the result to %8.8g\n",fresult); //	--> debug
							#endif														
							char buff2[256];
							char server[]="OK\n";
							memset(buff2,0,sizeof(buff2));
							//memset(buff3,0,sizeof(buff3));
							sprintf(buff2,"%8.8g\n",fresult);
							//printf("CONTENT OF BUFF2: %8.8g\n", buff2 ); //--> checking for gabbage	 		
			 				if(send(sockdes, buff2, sizeof(buff2), 0)==-1)
				 				{
								perror("fail, no float result sent...");
								exit(1);
				 				}
			 				else 
							{
                                //char buff2[256];
								memset(buff2,0,sizeof(buff2));
								//if(recv(sockdes, &buff2, strlen(buff2), 0)==-1)
								if(recv(sockdes, &buff2, sizeof(buff2), 0) ==-1 )	 
								{
									perror("No okay message frm server on receipt of my answer...");
									exit(1);
								}
								else
								{
									/*printf("am getting this ...%s...from server",buff2);*/
								}
									if(strcmp(buff2,server)==0)
		    						{
										//printf("%s\n",buff2);
										buff2[strcspn(buff2, "\n")] = 0;
										printf("%s (my result = %8.8g)\n",buff2,fresult);
									}
									else
									{
										perror("float value does not match...");
										exit(1);
									}
													 	 								 						
							}								 				
						}//end of float section 
						else 
						{
							//using string compare check the operator given and perform the random operation provided by the server
							// integer check				
							rv=sscanf(buff1,"%s %d %d",command,&i1,&i2);
							if (rv == EOF )
							{
								printf("Sscanf failed.\n");
								exit(1);
							}
							if(strcmp(command,"add")==0)
							{
								iresult=i1+i2;
							} 
							else if (strcmp(command, "sub")==0)
							{
								iresult=i1-i2;
							} 
							else if (strcmp(command, "mul")==0)
							{
								iresult=i1*i2;
							} 
							else if (strcmp(command, "div")==0)
							{     
								iresult=i1/i2;
							} 
							else 
							{
								printf("No match\n");
							}
							/*printf("%s %d %d = %d \n",command,i1,i2,iresult);*/ //--> checking my results
							#ifdef DEBUG 
							printf("Calculated the result to %d \n",iresult); 
							#endif
							char buff2[256];
							char buff3[256];
							char server1[]="OK\n";
							sprintf(buff2, "%d\n", iresult);
							//printf("CONTENT OF BUFF2: %s", buff2 ); // --> testing/debuging	 			
							if(send(sockdes,  buff2, strlen(buff2), 0)==-1)	
								{
								perror("fail, no integer result sent...");
								exit(1);
								}
							else
							{
									//if(recv(sockdes,  buff3, strlen(buff3), 0)==-1)	
									if(recv(sockdes, &buff3, sizeof(buff3), 0) ==-1 )
									{
									perror("No okay message frm server on receipt of my answer...");
									exit(1);
									}
									else
									{
										if(strcmp(buff3,server1)==0)
										{
										//printf("%s\n",buff3);
										buff3[strcspn(buff3, "\n")] = 0;
										printf("%s (my result = %d)\n",buff3,iresult);
										}	
										else
											{
											perror("server OK and my OK not equal...");
											exit(1);
											}					
									}
							}
						}//end of integer section for the command operation					
		}//opearator and values from server, end of else thereafter											
exit(sockdes);				
}//close main

  


 
 
