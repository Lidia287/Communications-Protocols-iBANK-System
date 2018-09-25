#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS	5
#define BUFLEN 256

struct persoana
{
	char nume[13];
	char prenume[13];
	int numar_card;
	int pin;
	char parola_secreta[10];
	double sold;
	int login;  // -1 nu e logat 1 e logat
	int unlock; // -1 e blocat  1 e deblocat
	int esuari;

};


//imi returneaza pozitia din vectorul de persoane a numarului de card indicat
int getClientIndex(struct persoana persoane[], int numar_card, int number)
{
	for(int i = 0 ; i < number ; i ++){
		if(persoane[i].numar_card == numar_card){
			return i;
		}
	}
	
	return -1;

}


void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

	//pentru UDP
	struct sockaddr_in my_sockaddr_udp, from_station_udp ;
	


	from_station_udp.sin_family = AF_INET;
	from_station_udp.sin_port = 0;
	from_station_udp.sin_addr.s_addr = htonl(INADDR_ANY);





	int i,j;
	FILE *fp;

	int sockClient[100];
	for(i = 0 ; i < 100; i ++){
		sockClient[i] = -1;
	}


	fp = fopen ("users_data_file", "r");
	
	int number;
	fscanf (fp,"%d\n", &number);
	//vectorul de persoane
	struct persoana *persoane = (struct persoana*)malloc(number * sizeof(struct persoana));;


	char stringHelper[100];
	j = 0;
	for(i = 0; i < number ; i++){
		
		fgets(stringHelper,100,fp);
		
		char *pch;
		pch = strtok (stringHelper," ");
		int index = 0;
  		while (pch != NULL)
 		 {
		
		if(index == 0){	
			
			strcpy(persoane[i].nume,pch);
			
			
		} else if(index == 1){
			strcpy(persoane[i].prenume,pch);
			
		} else if(index == 2){
			
			char* dst2;
			strcpy(&dst2,&pch);
			

			int val2 = atoi(dst2);
			
			persoane[i].numar_card = val2;


		} else if(index == 3){
			
			char* dst;
			strcpy(&dst,&pch);
			

			int val = atoi(dst);
			
			persoane[i].pin = val;
			
		} else if(index == 4){
			strcpy(persoane[i].parola_secreta,pch);
			
			
		} else if(index == 5){
			

			char* dst5;
			strcpy(&dst5,&pch);
			

			double val5 = atof(dst5);
			
			persoane[i].sold = val5;
			
		}
		
		index++;
		
    		pch = strtok (NULL, " ");
 		 }
		persoane[i].login = -1;
		persoane[i].unlock = 1;
		persoane[i].esuari = 0;
		j++;
		
	}	

	
   	fclose(fp);


	FILE *out;
	out = fopen ("logfile", "w");
	fclose(out);



     int sockfd, newsockfd, portno, clilen;
     char buffer[BUFLEN];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     fd_set read_fds;	//multimea de citire folosita in select()
     fd_set tmp_fds;	//multime folosita temporar 
     int fdmax;		//valoare maxima file descriptor din multimea read_fds

     if (argc < 2) {
         fprintf(stderr,"Usage : %s port\n", argv[0]);
         exit(1);
     }

     //golim multimea de descriptori de citire (read_fds) si multimea tmp_fds 
     FD_ZERO(&read_fds);
     FD_ZERO(&tmp_fds);


	int sockfd_udp = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); //socket pentru UDP
	
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     
     portno = atoi(argv[1]);

     memset((char *) &serv_addr, 0, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;	// foloseste adresa IP a masinii
     serv_addr.sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
              error("ERROR on binding");
     
	bind (sockfd_udp, (struct sockaddr_in*) &(serv_addr), sizeof(struct sockaddr));

     listen(sockfd, MAX_CLIENTS);

     //adaugam noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
     FD_SET(sockfd, &read_fds);

	FD_SET(sockfd_udp, &read_fds);

	FD_SET(0, &read_fds);
     fdmax = sockfd;


	char fileNames[100][BUFLEN];
	for(i = 0 ; i < 100; i ++){
		memset(fileNames[i], 0 , BUFLEN);
	}
	
	

	int ok_udp = 1;
 
	struct persoana currentClient;

	while (1) {
		int x = 0;
		tmp_fds = read_fds; 
		if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
			{printf("ERROR in select");}
	
		ok_udp = 1;
		

	
		for(i = 0; i <= fdmax; i++) {
			
			if (FD_ISSET(i, &tmp_fds)) {
			
				if (i == sockfd) {
					// a venit ceva pe socketul inactiv(cel cu listen) = o noua conexiune
					// actiunea serverului: accept()
					clilen = sizeof(cli_addr);
					if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1) {
						error("ERROR in accept");
					} 
					else {
						//adaug noul socket intors de accept() la multimea descriptorilor de citire
						FD_SET(newsockfd, &read_fds);
						if (newsockfd > fdmax) { 
							fdmax = newsockfd;
						}

						char temporar[BUFLEN];
						memset(temporar, 0, BUFLEN);

						if ((n = recv(newsockfd, temporar, sizeof(temporar), 0)) <= 0) {
						
						} else {
							
							strcpy(fileNames[newsockfd], temporar);
							
					
						}					


					}
				
				}

				else if(i == sockfd_udp){ //pentru operatia de unlock ce are lc pe socketul UDP
					int recv = 1;
					int count_recv_bytes = 0;
					int count_written_bytes = 0;
					int len = 10;
					int count_recv_bytes1 = 0;

					char file_name[BUFLEN];
					memset(file_name, 0, BUFLEN);


					count_recv_bytes1 = recvfrom (sockfd_udp, file_name, BUFLEN, 0, (struct sockaddr*) &from_station_udp, (socklen_t *) &len);

					memset(buffer, 0, BUFLEN);
				
					count_recv_bytes = recvfrom (sockfd_udp, buffer, BUFLEN, 0, (struct sockaddr*) &from_station_udp, (socklen_t *) &len);
					//printf("%s\n",buffer);

				
					char *pch;
					pch = strtok (buffer," ");
					int index = 0;

					int numar_card;
					

  					while (pch != NULL)
 					{
					
						if(index == 1){
							char* dst5;
							strcpy(&dst5,&pch);
			

							int val5 = atoi(dst5);
							numar_card = val5;
								
								
						} 
		
							
						index++;
							

							

    						pch = strtok (NULL, " ");
							
 		 			}


					int ind = getClientIndex(persoane, numar_card , number);
							
					if(ind == -1){
						out = fopen (file_name, "a");
						
						fprintf(out, "%s","UNLOCK> −4 : Numar card inexistent\n");
						fprintf(out, "%s","\n");
						fclose(out);
						char chq[BUFLEN];
						strcpy(chq, "UNLOCK> −4 : Numar card inexistent\n");
						
						int rc_udp = sendto(sockfd_udp, chq, strlen(chq), 0, (struct sockaddr *)&from_station_udp, sizeof(from_station_udp));
						if(rc_udp < 0) {
							perror("Send Error!\n");
							exit(-1);
						}

						


					} else if(ind != -1){
					
						if(persoane[ind].unlock == -1){
							out = fopen (file_name, "a");
							
							fprintf(out, "%s","UNLOCK> Trimite parola secreta\n");

					
							char chqw[BUFLEN];
							strcpy(chqw, "good");
							
							int rc_udp = sendto(sockfd_udp, chqw, strlen(chqw), 0, (struct sockaddr *)&from_station_udp, sizeof(from_station_udp));
							if(rc_udp < 0) {
								perror("Send Error!\n");
								exit(-1);
							}



							char pass[BUFLEN];
							
							int count_recv_bytes_pass = 0;
					
							int len_pass = 10;

							memset(pass, 0, BUFLEN);
							
							count_recv_bytes_pass = recvfrom (sockfd_udp, pass, BUFLEN, 0, (struct sockaddr*) &from_station_udp, (socklen_t *) &len_pass);
							

							
							char *pch;
							pch = strtok (pass," ");
							int index = 0;

							char parola_secreta[10];
  							while (pch != NULL)
 		 					{
		
								 if(index == 1){
									strcpy(parola_secreta,pch);
			
								} 
		
								index++;
		
    								pch = strtok (NULL, " ");
 							}

							
							fprintf(out, "%s",parola_secreta);
							


							if(strncmp(parola_secreta, persoane[ind].parola_secreta, strlen(persoane[ind].parola_secreta) - 1 ) == 0){
								
								fprintf(out, "%s","UNLOCK> Client deblocat\n");
								fprintf(out, "%s","\n");
								fclose(out);
								
								persoane[ind].unlock = 1;
								persoane[ind].esuari = 0;


							} else {
								
								fprintf(out, "%s","UNLOCK> −7 : Deblocare esuata\n");
								fprintf(out, "%s","\n");
								fclose(out);

							}

													


						} else if(persoane[ind].unlock == 1){
							out = fopen (file_name, "a");
							
							fprintf(out, "%s","UNLOCK> −6 : Operatie esuata\n");
							fprintf(out, "%s","\n");
							fclose(out);
							

							char chq[BUFLEN];
							strcpy(chq, "UNLOCK> −6 : Operatie esuata\n");
							
							int rc_udp = sendto(sockfd_udp, chq, strlen(chq), 0, (struct sockaddr *)&from_station_udp, sizeof(from_station_udp));
							if(rc_udp < 0) {
								perror("Send Error!\n");
								exit(-1);
							}


						}


					}

							




				}


					
				else if(i == 0){
	

					memset(buffer, 0, BUFLEN);

					fgets(buffer, BUFLEN, stdin);

					if(strncmp(buffer, "quit",4 ) == 0){ //quit pentru server
						
						x = 1; // variabila folosita si mai jos ca sa iasa si din while 
						break;  // imi iese din for,  programul se termina
					}
		
				}


				else {
					// am primit date pe unul din socketii cu care vorbesc cu clientii
					//actiunea serverului: recv()
					memset(buffer, 0, BUFLEN);

					
	
					if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
						if (n == 0) {
							//conexiunea s-a inchis
						
						} else {
							
						}
						close(i); 
						FD_CLR(i, &read_fds); // scoatem din multimea de citire socketul pe care 
					} 
					
					else { //recv intoarce >0
						
						if(buffer[0] == 'q' && buffer[1] == 'u' && buffer[2] == 'i' && buffer[3] == 't')
							{ //quit pentru client
							close(i);
							FD_CLR(i, &read_fds); // scoate clientul din lista
							
							out = fopen (fileNames[i], "a");
							fprintf(out, "%s","\n");
							fclose(out);

							break; 

							}
						if(buffer[0] == 'l' && buffer[1] == 'o' && buffer[2] == 'g' && buffer[3] == 'i' && buffer[4] == 'n'){

							//login - operatie pentru TCP

							char *pch;
							pch = strtok (buffer," ");
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
								
								
							} else if(index == 2){
								char* dst6;
								strcpy(&dst6,&pch);
			

								int val6 = atoi(dst6);
								pin = val6;
								

							}
		
							
							index++;
							

							

    							pch = strtok (NULL, " ");
							
 		 					}

							int ind = getClientIndex(persoane, numar_card , number);
							
							if(ind == -1){

								char good[BUFLEN] ;
								//good[0] = 'b';
								//good[1] = 'a';
								//good[2] = 'd';
								//good[3] = 'd';
								strcpy(good, "IBANK> −4 : Numar card inexistent\n");

								n = send(i,good,strlen(good), 0); 
    								if (n < 0) 
        				 				{error("ERROR writing to socket");}

								out = fopen (fileNames[i], "a");
								
								fprintf(out, "%s","IBANK> −4 : Numar card inexistent\n");
								fprintf(out, "%s","\n");
								fclose(out);
							} else if(ind != -1){
								

								if(persoane[ind].unlock == -1){

									char good[BUFLEN] ;
									//good[0] = 'b';
									//good[1] = 'a';
									//good[2] = 'd';
									//good[3] = 'd';
									strcpy(good, "IBANK> −5 : Card blocat\n");

									n = send(i,good,strlen(good), 0); 
    									if (n < 0) 
        				 					{error("ERROR writing to socket");}
									persoane[ind].unlock = -1;
												
									out = fopen (fileNames[i], "a");
									
									fprintf(out, "%s","IBANK> −5 : Card blocat\n");
									fprintf(out, "%s","\n");
									fclose(out);
								



								} else if(persoane[ind].unlock == 1){
									if(persoane[ind].login == 1){

										char good[BUFLEN] ;
										//good[0] = 'b';
										//good[1] = 'a';
										//good[2] = 'd';
										//good[3] = 'd';
										strcpy(good, "IBANK> −2 : Sesiune deja deschisa\n");

										n = send(i,good,strlen(good), 0); 
    										if (n < 0) 
        				 						{error("ERROR writing to socket");}

										out = fopen (fileNames[i], "a");
										
										fprintf(out, "%s","IBANK> −2 : Sesiune deja deschisa\n");
										fprintf(out, "%s","\n");
										fclose(out);

									} else if(persoane[ind].login == -1){
										

										if(persoane[ind].pin == pin){

											if(persoane[ind].esuari < 3){
												out = fopen (fileNames[i], "a");
				
												fprintf(out, "%s %s %s\n","IBANK> Welcome",persoane[ind].nume,persoane[ind].prenume);
												fprintf(out, "%s","\n");
												fclose(out);
								
												sockClient[i] = numar_card; 
												char good[5] ;
												good[0] = 'g';
												good[1] = 'o';
												good[2] = 'o';
												good[3] = 'd';
												

												n = send(i,good,strlen(good), 0); 
    												if (n < 0) 
        				 								{error("ERROR writing to socket");}
		
												
												persoane[ind].login = 1;
											} else if(persoane[ind].esuari >= 3){
												char good[BUFLEN] ;
												//good[0] = 'b';
												//good[1] = 'a';
												//good[2] = 'd';
												//good[3] = 'd';
												strcpy(good,"IBANK> −5 : Card blocat\n" );												

												n = send(i,good,strlen(good), 0); 
    												if (n < 0) 
        				 								{error("ERROR writing to socket");}
												persoane[ind].unlock = -1;
												
												out = fopen (fileNames[i], "a");
												
												fprintf(out, "%s","IBANK> −5 : Card blocat\n");
												fprintf(out, "%s","\n");
												fclose(out);
												
											}
										} else if(persoane[ind].pin != pin){
											persoane[ind].esuari = persoane[ind].esuari + 1;
											if(persoane[ind].esuari < 3){

												char good[BUFLEN] ;
												//good[0] = 'b';
												//good[1] = 'a';
												//good[2] = 'd';
												//good[3] = 'd';
												strcpy(good,"IBANK> −3 : Pin gresit\n" );												

												
												n = send(i,good,strlen(good), 0); 
    												if (n < 0) 
        				 								{error("ERROR writing to socket");}

												out = fopen (fileNames[i], "a");
												
												fprintf(out, "%s","IBANK> −3 : Pin gresit\n");
												fprintf(out, "%s","\n");
												fclose(out);

											} else if(persoane[ind].esuari >= 3){

												char good[BUFLEN] ;
												//good[0] = 'b';
												//good[1] = 'a';
												//good[2] = 'd';
												//good[3] = 'd';
												strcpy(good,"IBANK> −5 : Card blocat\n" );												
												

												n = send(i,good,strlen(good), 0); 
    												if (n < 0) 
        				 								{error("ERROR writing to socket");}
			
												persoane[ind].unlock = -1;
												out = fopen (fileNames[i], "a");
												
												fprintf(out, "%s","IBANK> −5 : Card blocat\n");
												fprintf(out, "%s","\n");
												fclose(out);
											}
										}

									}
								}
							}


						}



						if(buffer[0] == 'l' && buffer[1] == 'o' && buffer[2] == 'g' && buffer[3] == 'o' && buffer[4] == 'u' && buffer[5] == 't'){
						//logout - operatie pentru TCP
							
							int ind = getClientIndex(persoane, sockClient[i] , number);
							
							if(ind == -1){

								
							} else if(ind != -1){
								
								if(persoane[ind].unlock == 1){
									if(persoane[ind].login == 1){
										out = fopen (fileNames[i], "a");
										
										fprintf(out, "%s","IBANK> Clientul a fost deconectat\n");
										fprintf(out, "%s","\n");
										fclose(out);
										persoane[ind].login = -1;
									}
								}
							}


						}

						if(buffer[0] == 'l' && buffer[1] == 'i' && buffer[2] == 's' && buffer[3] == 't' && buffer[4] == 's' && buffer[5] == 'o' && buffer[6] == 'l' && buffer[7] == 'd'){

							//listsold - operatie pentru TCP

							int ind = getClientIndex(persoane, sockClient[i] , number);
							
							if(ind == -1){

								
							} else if(ind != -1){
								
								if(persoane[ind].unlock == 1){
									if(persoane[ind].login == 1){
										out = fopen (fileNames[i], "a");
										
										fprintf(out, "%s %.02lf\n","IBANK>",persoane[ind].sold);
										fprintf(out, "%s","\n");
										fclose(out);
										
									}
								}
							}


						}



						if(buffer[0] == 't' && buffer[1] == 'r' && buffer[2] == 'a' && buffer[3] == 'n' && buffer[4] == 's' && buffer[5] == 'f' && buffer[6] == 'e' && buffer[7] == 'r'){

							//transfer - operatie pentru TCP							

							char *pch;
							pch = strtok (buffer," ");
							int index = 0;

							int numar_card;
							double suma;

  							while (pch != NULL)
 							{
							
			
							if(index == 1){
								char* dst5;
								strcpy(&dst5,&pch);
			

								int val5 = atoi(dst5);
								numar_card = val5;
								
								
							} else if(index == 2){
								char* dst6;
								strcpy(&dst6,&pch);
			
								//printf(out, "%s",dst6);
								double val6 = atof(dst6);
								suma = val6;
								//printf(out, "%0.2lf",suma);
								

							}
		
							
							index++;
							

							

    							pch = strtok (NULL, " ");
							
 		 					}
							int trimit = getClientIndex(persoane, sockClient[i] , number);
							int ind = getClientIndex(persoane, numar_card , number);
							
							if(ind == -1){

								char good[5] ;
								good[0] = 'b';
								good[1] = 'a';
								good[2] = 'd';
								good[3] = 'd';
								

								n = send(i,good,strlen(good), 0); 
    								if (n < 0) 
        				 				{error("ERROR writing to socket");}

								out = fopen (fileNames[i], "a");
								
								fprintf(out, "%s","IBANK> −4 : Numar card inexistent\n");
								fprintf(out, "%s","\n");
								fclose(out);
							} else if(ind != -1){
								

									 
										

										if(persoane[trimit].sold >= suma){

												out = fopen (fileNames[i], "a");
						
												fprintf(out, "%s %.02lf %s %s %s%s","IBANK> Transfer",suma, "catre" , persoane[ind].nume, persoane[ind].prenume, "? [y/n]\n" );
												char good[5] ;
												good[0] = 'g';
												good[1] = 'o';
												good[2] = 'o';
												good[3] = 'd';
												
												n = send(i,good,strlen(good), 0); 
    												if (n < 0) 
        				 								{error("ERROR writing to socket");}
		
												memset(good, 0, 5);
												if ((n = recv(i, good, sizeof(good), 0)) <= 0) {
													if (n == 0) {
														//conexiunea s-a inchis
														
													} else {
														error("ERROR in recv");
													}
												} 
					
												else { 
													fprintf(out, "%s",good);

													if(strncmp(good, "y",1 ) == 0){

														

														persoane[trimit].sold = persoane[trimit].sold - suma;
														persoane[ind].sold = persoane[ind].sold + suma;
	
														fprintf(out, "%s","IBANK> Transfer realizat cu succes\n");
														fprintf(out, "%s","\n");
														fclose(out);


													} else {
	
														fprintf(out, "%s","IBANK> −9 : Operatie anulata\n");
														fprintf(out, "%s","\n");
														fclose(out);
													}
						
												}
											
										} else if(persoane[trimit].sold < suma){
											

												char good[5] ;
												good[0] = 'b';
												good[1] = 'a';
												good[2] = 'd';
												good[3] = 'd';
												
												out = fopen (fileNames[i], "a");
												fprintf(out, "%s","IBANK> −8 : Fonduri insuficiente\n");																															

												fprintf(out, "%s","\n");
												fclose(out);

												n = send(i,good,strlen(good), 0); 
    												if (n < 0) 
        				 								{error("ERROR writing to socket");}


												
	
												
											
										}

									
								}
							}

						
						

					}

				} 

			}

			
		}

	

			if(x == 1){
			break;   // imi iese si din while
			}
     }


     close(sockfd); 
     close(sockfd_udp);
	

	
   
     return 0; 
}


