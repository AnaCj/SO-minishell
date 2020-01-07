#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include "parser.h"


int auxFg (int total, tline * line, char * entrada, char * salida, char * error, bool ent, bool sal, bool err){
	
	// Declaracion de variables 
	int p[(total-1)][2];
	FILE * f1;
	FILE * f2;
	FILE * f;
	pid_t pid;
	int i;
	int j;
	int anterior;
	
	if (err){	// Si hay redireccion de error
		f2 = fopen (error,"w");
			if (f2 == NULL){	// Si se produce un error al abrir el fichero
				fprintf(stderr, "%s: Error. Error al abrir el fichero.\n", error);
			}
		dup2(fileno(f2),2);
		fclose(f2);
	}
	
	for (i=0; i<(total-1);i++){ 	// Crea de 0 a total-1 pipes
		pipe(p[i]);
	}
	for(i=0;i<total;i++){ 	// Para crear todos los hijos necesarios y realizar los mandatos

		if( signal(SIGINT, SIG_IGN) == SIG_ERR){	/* La shell no se cierra cuando hacemos ctrl + C */
			exit(-1);
		}
		if( signal(SIGQUIT, SIG_IGN) == SIG_ERR){	/* La shell no se cierra cuando hacemos ctrl + \ */
			exit(-1);
		}		
		
		anterior = i-1;
		pid = fork();	// Crea un hijo
		if (pid<0){	// Captura el error si no se crea bien el hijo
			fprintf(stderr, "Falló el fork().\n");
		}
		else if (pid==0){	// Hijo
			if (i==0){	// Si es el primer mandato
				if (ent){	// Si hay redirección de entrada
					f1 = fopen (entrada,"r");
					dup2(fileno(f1),STDIN_FILENO);
					fclose(f1);
				}
				if ((i+1)!=total){	// Si no es el ultimo mandato
					dup2(p[i][1],1);	//Redirecciona su salida al pipe
					for (j=0;j<(total-1);j++){	// Cierra los pipes que no usa
						if (j!=i){
							close(p[j][0]);
							close(p[j][1]);
						}
					}
					close(p[i][0]);
				}
				else {	// Es el ultimo mandato
					if (sal){	// Si hay redireccion de salida
						f2 = fopen (salida,"w");
						if (f2 == NULL){	// Si se produce un error al abrir el fichero
							fprintf(stderr, "%s: Error al abrir el fichero.\n", salida);
						}
						dup2(fileno(f2),STDOUT_FILENO);
						fclose(f2);
					}
				}
			}
			else if ((i>0)&&(i<(total-1))) {	// Si es no es ni el primer ni el ultimo mandato
				dup2(p[anterior][0],0);		// Redirecciona su entrada y salida a los pipes
				dup2(p[i][1],1);
				for (j=0;j<(total-1);j++){	// Cierra los pipes que no usa
					if ((j!=i)&&(j!=anterior)){
						close(p[j][0]);
						close(p[j][1]);
					}
				}
				close(p[i][0]);
				close(p[anterior][1]);
			}
			else {	// Si es el ultimo mandato
				dup2(p[anterior][0],0);		// Redirecciona su entrada al pipe
				for (j=0;j<(total-1);j++){	// Cierra los pipes que no usa
					if (j!=anterior){
						close(p[j][0]);
						close(p[j][1]);
					}
				}
				close(p[anterior][1]);
				if (sal){	// Si hay redirección de salida
					f2 = fopen (salida,"w");
					if (f2 == NULL){	// Si hay un error al abrir el fichero
						fprintf(stderr, "%s: Error al abrir el fichero.\n", salida);
					}
					dup2(fileno(f2),STDOUT_FILENO);
					fclose(f2);
				}
			}
			execvp (line->commands[i].filename, line->commands[i].argv);	// Ejecuta el mandato
			fprintf(stderr, "Falló el exec del comando \"%s\".\n",line->commands[i].filename);
			
		}
		else {	// Padre
			if (line->commands[i].filename==NULL){		// Comprueba que el comando existe
				fprintf(stderr,"%s: No se encuentra el mandato.\n", line->commands[i].argv[0]);
				return 0;	// Si no existe salimos de la funcion
			}
			if (ent){	// Si hay redirección de entrada
				f = fopen (entrada,"r");
				if (f == NULL){	// Si hay un error al abrir el fichero
					fprintf(stderr, "%s: Error. El fichero no existe.\n", entrada);
					return 0;
				}
			}
			if (i==(total-1)){	// Cuando ha acabado de crear hijos
				for (j=0;j<(total-1);j++){	// Cierra los pipes que no usa (todos)
					close(p[j][0]);
					close(p[j][1]);
				}
			}
		}
	}
	for (i=0;i<total;i++){	// Espera a que terminen los hijos
		wait(NULL);
	}
	return 0;	
}

int auxBg (int total, tline * line, char * entrada, char * salida, char * error, bool ent, bool sal, bool err){

	// Declaracion de variables 
	int p[(total-1)][2];
	FILE * f1;
	FILE * f2;
	FILE * f;
	pid_t pid;
	int i;
	int j;
	int anterior;
	
	if (err){	// Si hay redireccion de error
		f2 = fopen (error,"w");
			if (f2 == NULL){	// Si se produce un error al abrir el fichero
				fprintf(stderr, "%s: Error. Error al abrir el fichero.\n", error);
			}
		dup2(fileno(f2),2);
		fclose(f2);
	}
	
	for (i=0; i<(total-1);i++){ 	// Crea de 0 a total-1 pipes
		pipe(p[i]);
	}
	for(i=0;i<total;i++){ 	// Para crear todos los hijos necesarios y realizar los mandatos

		if( signal(SIGINT, SIG_IGN) == SIG_ERR){	/* La shell no se cierra cuando hacemos ctrl + C */
			exit(-1);
		}
		if( signal(SIGQUIT, SIG_IGN) == SIG_ERR){	/* La shell no se cierra cuando hacemos ctrl + \ */
			exit(-1);
		}		
		
		anterior = i-1;
		pid = fork();	// Crea un hijo
		if (pid<0){	// Captura el error si no se crea bien el hijo
			fprintf(stderr, "Falló el fork().\n");
		}
		else if (pid==0){	// Hijo
			if (i==0){	// Si es el primer mandato
				if (ent){	// Si hay redirección de entrada
					f1 = fopen (entrada,"r");
					dup2(fileno(f1),STDIN_FILENO);
					fclose(f1);
				}
				if ((i+1)!=total){	// Si no es el ultimo mandato
					dup2(p[i][1],1);	//Redirecciona su salida al pipe
					for (j=0;j<(total-1);j++){	// Cierra los pipes que no usa
						if (j!=i){
							close(p[j][0]);
							close(p[j][1]);
						}
					}
					close(p[i][0]);
				}
				else {	// Es el ultimo mandato
					if (sal){	// Si hay redireccion de salida
						f2 = fopen (salida,"w");
						if (f2 == NULL){	// Si se produce un error al abrir el fichero
							fprintf(stderr, "%s: Error al abrir el fichero.\n", salida);
						}
						dup2(fileno(f2),STDOUT_FILENO);
						fclose(f2);
					}
				}
			}
			else if ((i>0)&&(i<(total-1))) {	// Si es no es ni el primer ni el ultimo mandato
				dup2(p[anterior][0],0);		// Redirecciona su entrada y salida a los pipes
				dup2(p[i][1],1);
				for (j=0;j<(total-1);j++){	// Cierra los pipes que no usa
					if ((j!=i)&&(j!=anterior)){
						close(p[j][0]);
						close(p[j][1]);
					}
				}
				close(p[i][0]);
				close(p[anterior][1]);
			}
			else {	// Si es el ultimo mandato
				dup2(p[anterior][0],0);		// Redirecciona su entrada al pipe
				for (j=0;j<(total-1);j++){	// Cierra los pipes que no usa
					if (j!=anterior){
						close(p[j][0]);
						close(p[j][1]);
					}
				}
				close(p[anterior][1]);
				if (sal){	// Si hay redirección de salida
					f2 = fopen (salida,"w");
					if (f2 == NULL){	// Si hay un error al abrir el fichero
						fprintf(stderr, "%s: Error al abrir el fichero.\n", salida);
					}
					dup2(fileno(f2),STDOUT_FILENO);
					fclose(f2);
				}
			}
			execvp (line->commands[i].filename, line->commands[i].argv);	// Ejecuta el mandato
			fprintf(stderr, "Falló el exec del comando \"%s\".\n",line->commands[i].filename);
			
		}
		else {	// Padre
			if (line->commands[i].filename==NULL){		// Comprueba que el comando existe
				fprintf(stderr,"%s: No se encuentra el mandato.\n", line->commands[i].argv[0]);
				return 0;	// Si no existe salimos de la funcion
			}
			if (ent){	// Si hay redirección de entrada
				f = fopen (entrada,"r");
				if (f == NULL){	// Si hay un error al abrir el fichero
					fprintf(stderr, "%s: Error. El fichero no existe.\n", entrada);
					return 0;
				}
			}
			if (i==(total-1)){	// Cuando ha acabado de crear hijos
				for (j=0;j<(total-1);j++){	// Cierra los pipes que no usa (todos)
					close(p[j][0]);
					close(p[j][1]);
				}
			}
		}
	}

	waitpid(pid,NULL,WNOHANG);	// Wait no bloqueante
	return pid;	
}

int main(void) {

	//Declaracion de variables
	bool ent;
	bool sal;
	bool err;
	char * entrada;
	char * salida;
	char * error;
	tline * line;
	char buf [1024];
	FILE * f;
	int bg[50];
	char bgCommands[50][1024];
	int i;
	int j;	
	int cont;
	
	cont=0;

	printf("msh> ");	

	if( signal(SIGINT, SIG_IGN) == SIG_ERR){
			exit(-1);
		}
	if( signal(SIGQUIT, SIG_IGN) == SIG_ERR){
			exit(-1);
		}

	while (fgets(buf, 1024, stdin)) {	// Bucle de la shell
	
		if( signal(SIGINT, SIG_IGN) == SIG_ERR){
			exit(-1);
		}
		if( signal(SIGQUIT, SIG_IGN) == SIG_ERR){
			exit(-1);
		}
		
		ent=false;
		sal=false;
		err=false; 
		
		line = tokenize(buf);
		if (line==NULL) {	// Si no hay mandatos
			continue;
		}
		if (line->redirect_input != NULL) {	// True si hay redirección de entrada
			ent=true;
			entrada = line->redirect_input;
		}
		if (line->redirect_output != NULL) {	// True si hay redirección de salida
			sal=true;
			salida = line->redirect_output;
		}
		if (line->redirect_error != NULL) {	// True si hay redirección de error
			err=true;
			error = line->redirect_error;
		}
		if ((line->ncommands==1)&&(line->commands[0].filename==NULL)){	// Si el mandato que llega no existe
			if (strcmp(line->commands[0].argv[0],"jobs")==0){	// Ejecutamos el jobs
				for(i=0; i<cont; i++){
					printf ("[%i] Ejecutando: %s", i+1,bgCommands[i]);
				}
			}
			else if (strcmp(line->commands[0].argv[0],"fg")==0){	// Ejecutamos el fg
				i = atoi(line->commands[0].argv[1]);
				for (j=0;j<1024;j++){
					if (bgCommands[i-1][j]!='&'){
						printf("%c", bgCommands[i-1][j]);
					}
				}
				waitpid(bg[i-1],NULL,0);
			}
			else {
				if (err){	// Redireccion de error
					f = fopen (error,"w");
					dup2(fileno(f),2);
					fclose(f);
				}
				fprintf(stderr, "%s: No se encuentra el mandato.\n", line->commands[0].argv[0]);
			}
		}
		else if (line->background) {	// Llamada a la función que realiza mandatos en background
			if (cont==50){	// Si se llega al final del array volvemos al principio
				cont = 0;
			}
			bg[cont]=auxBg(line->ncommands, line, entrada, salida, error, ent, sal, err); 
			strcpy(bgCommands[cont],buf);
			cont++;
			printf ("[%i] %i\n", cont, bg[cont-1]);
		}
		else if (!line->background){	// Llamada a la función que realiza los mandatos en foreground
			auxFg(line->ncommands, line, entrada, salida, error, ent, sal, err);
		}
		printf("msh> ");
	}
	return 0;
}
