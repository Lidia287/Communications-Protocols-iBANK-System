#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
 #include <unistd.h>

#define BUFLEN 256

void error(char *msg)
{
    perror(msg);
    exit(0);
}

struct persoana
{
	char* nume;
	char* prenume;
	int numar_card;
	int pin;
	char* parola_secreta;
	double sold;
	int login;  // -1 nu e logat 1 e logat
	int unlock; // -1 e blocat  1 e deblocat

};



int main(int argc, char *argv[])
{


	


	/*Deschidere socket*/
	int sockfd_udp = socket(AF_INET, SOCK_DGRAM, 0); //socketul pentru UDP
	if(sockfd_udp < 0) {
		perror("Init Error!\n");
	}

    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFLEN];
    if (argc < 3) {
       fprintf(stderr,"Usage %s server_address server_port\n", argv[0]);
       exit(0);
    }  
    
	sockfd = socket(AF_INET, SOCK_STREAM, 0);   //socketul pt TCP
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &serv_addr.sin_addr);
    
    
    if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");   



 	fd_set read_fds;	
    
	FD_ZERO(&read_fds);
     
 	FD_SET(sockfd, &read_fds);
 	FD_SET(0, &read_fds);
	FD_SET(sockfd_udp, &read_fds); 

	fd_set tmp_fds;
	int fdmax = 9;
	int i;

	int numar_card_global = -1;

	char nr_card_global[100] ;


	int current = -1;
    

	int fd_udp;
	
	


	FILE *out; //fisierul in care voi scrie

	//printf("%d\n",getpid());
	char nume_log[BUFLEN];
	memset(nume_log, 0 ,BUFLEN);

	sprintf(nume_log, "client-%d.log",getpid());
	
	FILE *fisier_log = fopen(nume_log, "w");
	fclose(fisier_log);

	n = send(sockfd,nume_log,strlen(nume_log), 0); 
    	if (n < 0) 
        	{error("ERROR writing to socket");}


	

    while(1){
  		


		tmp_fds = read_fds; 
		if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
			{printf("ERROR in select");}
		
		int x = 0;
		for(i = 0; i <= fdmax; i++) {


			if (FD_ISSET(i, &tmp_fds)) {
			
				if (i == sockfd) {

					int m;
					if ((m = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) { 

				

					}


				}
				else if(i == 0){ 

					memset(buffer, 0 , BUFLEN);
		
    					fgets(buffer, BUFLEN-1, stdin);  //comanda pe care o voi trimite la server
					out = fopen (nume_log, "a");
					fprintf(out, "%s",buffer);
					fclose(out);

					


					if(buffer[0] == 'q' && buffer[1] == 'u' && buffer[2] == 'i' && buffer[3] == 't'){
						
						x = 1;
						break; // imi iese din for
					}

					
					if(buffer[0] == 'u' && buffer[1] == 'n' && buffer[2] == 'l' && buffer[3] == 'o' && buffer[4] == 'c' && buffer[5] == 'k'){
					
					//unlock - operatie pentru UDP
				
					int rc_udp1 = sendto(sockfd_udp, nume_log, strlen(nume_log), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
					if(rc_udp1 < 0) {
						perror("Send Error!\n");
						exit(-1);
					} //trimit la server cererea de unlock



					char buf[BUFLEN];

					strcpy(buf, "unlock ");


					char snum[100];
					
					snprintf (snum, sizeof(snum), "%d",numar_card_global);


					strcat(buf, snum);

					
					strcat(buf, "\n");

					//printf("%s\n",buf);
					int rc_udp = sendto(sockfd_udp, buf, strlen(buf), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
					if(rc_udp < 0) {
						perror("Send Error!\n");
						exit(-1);
					} //trimit la server cererea de unlock

					//printf("%s\n",buf);


					int recv = 1;
					int count_recv_bytes = 0;
					int count_written_bytes = 0;
					int len = 10;

					memset(buf, 0, BUFLEN);

					char chh[BUFLEN];

					memset(chh, 0, BUFLEN);

					
					count_recv_bytes = recvfrom (sockfd_udp, chh, BUFLEN, 0, (struct sockaddr*) &serv_addr, (socklen_t *) &len);
					
				
					if(strncmp(chh, "good",4 ) == 0){ //primesc de la server good daca era blocat contul 
				
						char parola[BUFLEN];
						fgets(parola, BUFLEN-1, stdin); //trimit inapoi o parola secreta 
						


						char sndPass[BUFLEN];

						strcpy(sndPass, "");

						strcat(sndPass,snum);

						strcat(sndPass, " ");

					
						strcat(sndPass, parola);

					


						int rc_udp_pass = sendto(sockfd_udp, sndPass, strlen(sndPass), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
						if(rc_udp_pass < 0) {
							perror("Send Error!\n");
							exit(-1);
						}



					}

					}


					 if(buffer[0] == 'l' && buffer[1] == 'o' && buffer[2] == 'g' && buffer[3] == 'i' && buffer[4] == 'n'){

							
							//login - operatie pentru TCP

							char *pch;
							char buff[BUFLEN];
							strcpy(buff,buffer);
							pch = strtok (buff," ");
							int index = 0;

							int numar_card;
							int pin;

  							while (pch != NULL)
 							{
							
			
							if(index == 1){
								char* dst5;
								strcpy(&dst5,&pch);
			

								int val5 = atoi(dst5);
								
								numar_card = val5;
								numar_card_global = val5;

								
								
								
							} else if(index == 2){
								char* dst6;
								strcpy(&dst6,&pch);
			

								int val6 = atoi(dst6);
								pin = val6;
								

							}
		
							
							index++;
							

							

    							pch = strtok (NULL, " "); //pentru parsarea comenzii
							
 		 					}
							
							
							if(current != -1){ //sesiunea era deja deschisa

								out = fopen (nume_log, "a");
								fprintf(out, "%s","-2 : Sesiune deja deschisa\n");
								fprintf(out,"%s","\n");
								fclose(out);

								
	
							} else if(current == -1){
								
								
								//trimit mesaj la server
    								n = send(sockfd,buffer,strlen(buffer), 0); 
    								if (n < 0) 
        				 				{error("ERROR writing to socket");}
								
								memset(buffer, 0, BUFLEN);
								if ((n = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0) {
									if (n == 0) {
										//conexiunea s-a inchis
										
									} else {
										error("ERROR in recv");
									}
								} 
					
								else { 
									if(strncmp(buffer, "good",4 ) == 0){
										current = numar_card; 
										//daca serverul mi-a trimis inapoi ca s-a logat
									}
						
								}
								
							}


					} 



					if(buffer[0] == 'l' && buffer[1] == 'o' && buffer[2] == 'g' && buffer[3] == 'o' && buffer[4] == 'u' && buffer[5] == 't'){

							//logout - operatie pentru TCP
							

							if(current != -1){
								
								current = -1;
								//trimit mesaj la server
    								n = send(sockfd,buffer,strlen(buffer), 0);
    								if (n < 0) 
        				 				{error("ERROR writing to socket");}
	
							} else if(current == -1){
								
							
								out = fopen (nume_log, "a");
								fprintf(out, "%s","-1 : Clientul nu este autentificat\n");
								fprintf(out,"%s","\n");
								fclose(out);
								
								
							}


					} 

					if(buffer[0] == 'l' && buffer[1] == 'i' && buffer[2] == 's' && buffer[3] == 't' && buffer[4] == 's' && buffer[5] == 'o' && buffer[6] == 'l' && buffer[7] == 'd'){

							
							//listsold - operatie pentru TCP

							if(current != -1){
								
								
								//trimit mesaj la server
    								n = send(sockfd,buffer,strlen(buffer), 0);
    								if (n < 0) 
        				 				{error("ERROR writing to socket");}
	
							} else if(current == -1){
								
								
								out = fopen (nume_log, "a");
								fprintf(out, "%s","-1 : Clientul nu este autentificat\n");
								fprintf(out,"%s","\n");
								fclose(out);	
								
								
							}


					} 



					if(buffer[0] == 't' && buffer[1] == 'r' && buffer[2] == 'a' && buffer[3] == 'n' && buffer[4] == 's' && buffer[5] == 'f' && buffer[6] == 'e' && buffer[7] == 'r'){

							
							//transfer - operatie pentru TCP

							if(current != -1){
								
								
								//trimit mesaj la server
    								n = send(sockfd,buffer,strlen(buffer), 0);
    								if (n < 0) 
        				 				{error("ERROR writing to socket");}

								memset(buffer, 0, BUFLEN);
								if ((n = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0) {
									if (n == 0) {
									//conexiunea s-a inchis
										
									} else {	
										
										error("ERROR in recv");
										}
									} 
					
								else { 
									if(strncmp(buffer, "good",4 ) == 0){

									//trimit y - adica scriu de la tastatura
									memset(buffer, 0 , BUFLEN);
		
    									fgets(buffer, BUFLEN-1, stdin);

									//trimit mesaj la server
    									n = send(sockfd,buffer,strlen(buffer), 0);
    									if (n < 0) 
        				 					{error("ERROR writing to socket");}


									} else{
									//nu a fost bine 
										}
						
									}
								

	
							} else if(current == -1){
								
								
								out = fopen (nume_log, "a");
								fprintf(out, "%s","-1 : Clientul nu este autentificat\n");
								fprintf(out,"%s","\n");
								fclose(out);
								
								
							}


					}
					
	



				}
			}
		

		}

		if(x == 1){
			break;  // imi iese si din while
		}

    }

	close(sockfd);
	close(sockfd_udp);

	

    return 0;
}


