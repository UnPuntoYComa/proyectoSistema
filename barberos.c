#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>          
pthread_t hiloBarbero;           // hilo de barbero
pthread_t hilosClientes[100];   // hilo de clientes max 100
sem_t asientoMutex;        // Ssemaforo para el asiento
sem_t clientes;        // semafaro para los clientes
sem_t semaforoBarbero;        // semafaro para el barbero
int tiempo;        // tiempo en segundo que trabajará
int asientosLibres;  // cantidad de asientos
int contadorCli;     // numero de clientes que estaran
int tiempoBarberos;       //tiempo estimado para que va ver si está libre un asiento 
int contadorCortesHechos;             // clientes satisfechos
void *barbero() {
    int trabajando;    // tiempo haciendo el corte
	FILE *filePointer;
    while(1) { 
	    sem_wait(&clientes);   
	    sem_wait(&asientoMutex);// espera por el asiento
	    //zona critica
	    asientosLibres += 1;  
	    trabajando = (rand() % 5) + 1; //numero random de 1 a 5
        filePointer = fopen("salida.txt","a"); //escribe en este archivo
	    printf("Barbero atiende a nuevo cliente, tomará %d segundos \n",trabajando); 
	    fprintf(filePointer,"Barbero atiende a nuevo cliente, tomará %d segundos \n",trabajando);
	    printf("\tnumero de asientos libres: %d\n",asientosLibres);
	    fprintf(filePointer,"\tnumero de asientos libres: %d\n",asientosLibres);
        fclose(filePointer);
        // semaforo que el barbero esta listo por ser atendido
	    sem_post(&semaforoBarbero);
        // libera el asiento
	    sem_post(&asientoMutex);
   	    sleep(trabajando);
    } 
}

void *cliente() {
    int tiempoEspera;
    FILE *filePointer;
    int final = 1;
    while(final == 1) {
        // acceso a un asiento  
	    sem_wait(&asientoMutex);
        //cuando no hay asientos 
	    if(asientosLibres <= 0){
	        tiempoEspera = (rand() % tiempoBarberos) + 1;
		    printf("cliente %lu se fue sin corte, y regresará despues de %d segundos\n", pthread_self(),tiempoEspera);
            filePointer = fopen("salida.txt","a");
	        fprintf(filePointer,"cliente %lu se fue sin corte, y regresará despues de %d segundos\n", pthread_self(),tiempoEspera);
            fclose(filePointer);
		    sem_post(&asientoMutex); //libera el semaforo
	        sleep(tiempoEspera);
	    }
        // cuando hay asientos libre sucede esto
	    else{ 
            // decrementa el numero de asientos disponibles 
		    asientosLibres -= 1;
		    printf("cliente %lu está esperando.\n",pthread_self());
		    printf("\tnumero de acientos libres: %d\n",asientosLibres);
            filePointer = fopen("salida.txt","a");
	        fprintf(filePointer,"cliente %lu está esperando.\n",pthread_self());
	        fprintf(filePointer,"\tnumero de acientos libres: %d\n",asientosLibres);
            fclose(filePointer);
            // el cliente esta listo 
		    sem_post(&clientes);
            // libera el asiento de la sala de espera
		    sem_post(&asientoMutex);
            // espera por el bambero
		    sem_wait(&semaforoBarbero);
            // le hace un corte el barbero
		    printf("Cliente %lu tiene un corte de pelo\n",pthread_self());
            FILE *filePointer;
            filePointer = fopen("salida.txt","a");
	        fprintf(filePointer,"Cliente %lu tiene un corte de pelo\n",pthread_self());
            fclose(filePointer);
		    //sale del bucle 
		    final = 0;
		    // al salir cliente se incrementa los cortes hecho
		    contadorCortesHechos += 1;
	    }
      
     }
     }

int main() {
    printf("tiempo corriendo en el sistema: ");
    scanf("%d",&tiempo);
    printf("agregue numero de acientos: ");
    scanf("%d",&asientosLibres);
    printf("agregue la cantidad de clientes: ");
    scanf("%d",&contadorCli);
    printf("agregue el tiempo para que el cliente regrese: ");
    scanf("%d",&tiempoBarberos);
    
    contadorCortesHechos = 0;   
   
    printf("\ninicio de el sistema\n");
   	FILE *filePointer;
    filePointer = fopen("salida.txt","w");
    fclose(filePointer);
    // iniciamos los semaforos
    sem_init(&clientes,0,0);
    sem_init(&semaforoBarbero,0,0);
    sem_init(&asientoMutex,0,1);
   
    // crea el hilo de el barbero 
    pthread_create(&hiloBarbero, NULL, barbero, NULL); //barbero es la funcion que hace al crearse
    printf("barbero iniciado\n");
    
    // Creating clientes threads:
    for (int i = 0; i < contadorCli; i++){
	   pthread_create(&hilosClientes[i], NULL, cliente, NULL); 
	   printf("cliente %lu fue creado\n",hilosClientes[i]);
    }
    // detenemos el main 
    sleep(tiempo);
    filePointer = fopen("salida.txt","a");
	fprintf(filePointer,"\n\nfin del dia\n");
	fprintf(filePointer,"%d cortes de %d hechos",contadorCortesHechos,contadorCli);
    fclose(filePointer);
    printf("\n\nfin del dia\n");
    printf("%d cortes de %d hechos",contadorCortesHechos,contadorCli);
    exit(0);
}