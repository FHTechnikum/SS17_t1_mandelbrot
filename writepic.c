/* !* WRITEPICTURE PROCESS
 *
 * \description Reads the pixels from pixelgens shared memory and writes them
 *              into a ppm P3 picture
 *
 *
 * \author Sebastian Dichler <el16b032@technikum-wien.at> <sedi343@gmail.com>
 *
 * \version Rev.: 01, 23.03.2017 - Created
 *          Rev.: 02, 30.03.2017 - Added semaphore1, semaphore2 and sharedmemory
 *          Rev.: 03, 30.03.2017 - Added sharedmemory read
 *                                 -> add while(1), SIGNAL, and SIGNAl handler
 *          Rev.: 04, 01.04.2017 - Writing the CNTRL+C handler and while(1) loop
 *          Rev.: 05, 01.04.2017 - Should be done now, but somehow the pixelgen is buggy
 *
 
MANDELBROT @ v1.0
Created by Sebastian Dichler, 2017
Use"-?" for more information.

*** DEBUG MODE ACTIVE ***

Start client now...

Semaphore ID1: 196608
Semaphore ID2: 196608
Key SharedMem: 1645281434
Key Semaphore: 1645281434

Width: 800
Height: 600









MANDELBROT @ v1.0
Created by Sebastian Dichler, 2017
Use"-?" for more information.

*** DEBUG MODE ACTIVE ***

*** Waiting for data from client... ***

* Writing file...
* Done writing file!

Wrote file within 0.174660 secs

* Writing file...
* Done writing file!

Wrote file within 0.182246 secs

^C
You just typed CTRL+C
Server is closing everything...

ERROR: semctl: Can't control Semaphore 2, continue...: Invalid argument

ERROR: msctl: Can't close Message 2, continue...: Invalid argument
If any error appeared check it manually with ipcs and remove them with ipcrm
 
 *
 * \information CNTRL+C handler with help of Helmut Resch
 *
 *
 */

#include "myhead.h"

/* ---- GLOBAL VARIABLES ---- */

key_t keySemaphore;
key_t keySharedMem;
key_t keymsg;

SEMUN semaphore1union;
SEMUN semaphore2union;
SEMBUF semaphore1buffer;
SEMBUF semaphore2buffer;

int semaphore1;
int semaphore2;
int sharedmemid;
long int msqid1 = 0;
long int msqid2 = 0;
long int msgtype = 0;

#if TIME 
struct timeval timer1, timer2;
#endif

/* ---- FUNCTION DECLARATION ---- */

void cntrl_c_handler_server(int dummy);

/* ---- MAIN FUNCTION ---- */
 
 int main(int argc, char *argv[])
 {
	int width;
	int height;
	
	FILE* pFout = NULL;
	
 	PICTURE *picture_Pointer_global = NULL;
	
	int k = 0;
	int i;
	int error = 0;
	
	char filename[STRINGLENGTH];
	
#if TIME
	double timediff;
#endif
	
/*------------------------------------------------------------------*/
/* I N I T                                                          */
/*------------------------------------------------------------------*/
	
	clear();
	
	printf(BOLD"Start client now...\n\n"RESET);
	
/* ---- GENERATE KEYS FOR SEMAPHORE AND SHARED MEMORY ---- */
	
	keySemaphore = ftok("/etc/hostname", 'b');
	if (keySemaphore == -1)
	{
		perror(BOLD"\nERROR: ftok: Can't generate Semaphore Key"RESET);
		exit(EXIT_FAILURE);
	}
	
	keySharedMem = ftok("/etc/hostname", 'b');
	if (keySharedMem == -1)
	{
		perror(BOLD"\nERROR: ftok: Can't generate Shared-Memory Key"RESET);
		exit (EXIT_FAILURE);
	}
	
/* ---- RCV WIDTH AND HEIGHT FROM PIXELGEN ---- */
	
	keymsg = ftok("/etc/hostname", 'b');
	if (keymsg == -1)
	{
		perror(BOLD"\nERROR: ftok: Can't generate Message Key"RESET);
		exit(EXIT_FAILURE);
	}
	
	msqid1 = msgget(keymsg, IPC_CREAT | 0666);
	if (msqid1 >= 0)
	{
		if (msgrcv(msqid1, &width, sizeof(width), msgtype, 0) == -1)
		{
			perror(BOLD"\nERROR: msgsnd: Can't read width"RESET);
			exit(EXIT_FAILURE);
		}
	}
	
	msqid2 = msgget(keymsg, IPC_CREAT | 0666);
	if (msqid2 >= 0)
	{
		if(msgrcv(msqid2, &height, sizeof(height), msgtype, 0) == -1)
		{
			perror(BOLD"\nERROR: msgsnd: Can't read height"RESET);
			exit(EXIT_FAILURE);
		}
	}
	
/* ---- GENERATE SHARED MEMORY ---- */
	
	sharedmemid = shmget(keySharedMem, (width * height * sizeof(PICTURE)), IPC_CREAT | 0666);
	if (sharedmemid == -1)
	{
		perror(BOLD"\nERROR: shmget: Couldn't generate Shared-Memory."RESET);
		exit(EXIT_FAILURE);
	}
	
/* ---- GENERATE SEMAPHORE 1 ---- */
	
	semaphore1 = semget(keySemaphore, 1, IPC_CREAT | 0666);
	if (semaphore1 < 0)
	{
		perror(BOLD"\nERROR: semget: Couldn't generate Semaphore 1"RESET);
		exit(EXIT_FAILURE);
	}
	
/* ---- GENERATE SEMAPHORE 2 ---- */
	
	semaphore2 = semget(keySemaphore, 1, IPC_CREAT | 0666);
	if (semaphore2 < 0)
	{
		perror(BOLD"\nERROR: semget: Couldn't generate Semaphore 2"RESET);
		exit(EXIT_FAILURE);
	}
	
/*------------------------------------------------------------------*/
/* P R O G R A M M   S T A R T                                      */
/*------------------------------------------------------------------*/
	
/* ---- USER OUTPUT ---- */

	clear();
	
#if DEBUG
	printf(BOLDRED"Semaphore ID1: %d\n"RESET, semaphore1);
	printf(BOLDRED"Semaphore ID2: %d\n"RESET, semaphore2);
	printf(BOLDRED"Key SharedMem: %d\n"RESET, keySharedMem);
	printf(BOLDRED"Key Semaphore: %d\n\n"RESET, keySemaphore);
	
	printf(BOLDRED"Width: %d\n"RESET, width);
	printf(BOLDRED"Height: %d\n\n"RESET, height);
#endif
	
	printf(BOLD ITALIC"*** Waiting for data from client... ***\n\n"RESET);
	
/* ---- OPNE SEMAPHORE 1 ---- */
	
	semaphore1union.val = 1;
	
	if (semctl(semaphore1, 0, SETVAL, semaphore1union) < 0)
	{
		perror(BOLD"\nERROR: semctl: Can't control Semaphore 1"RESET);
		exit(EXIT_FAILURE);
	}
	
/* ---- CLOSE SEMAPHORE 2 ---- */
	
	semaphore2union.val = 0;
	
	if (semctl(semaphore2, 0, SETVAL, semaphore2union) < 0)
	{
		perror(BOLD"\nERROR: semctl: Can't control Semaphore 2"RESET);
		exit(EXIT_FAILURE);
	}

/* ---- CTRL+C HANDLER ---- */
	
	signal(SIGINT, cntrl_c_handler_server);
	
/* ---- ALGORITHM START ---- */
	
	while(1)
	{
		
/* ---- ATTACH SHARED MEMORY ---- */
		
		picture_Pointer_global = shmat(sharedmemid, 0, 0);
		if (picture_Pointer_global == (PICTURE *)-1)
		{
			perror(BOLD"ERROR: shamt: Can't attach Shared-Memory"RESET);
			exit(EXIT_FAILURE);
		}
		
/* ---- REQUEST ACCESS TO SEMAPHORE 2 ---- */
		
		semaphore2buffer.sem_num = 0;
		semaphore2buffer.sem_op = -1;
		semaphore2buffer.sem_flg = 0;
		
		if (semop(semaphore2, &semaphore2buffer, 1) == -1)
		{
			perror(BOLD"\nERROR: semop: Can't unlock Semaphore 2"RESET);
			exit(EXIT_FAILURE);
		}
			
#if TIME
		gettimeofday(&timer1, NULL);
#endif
		
		printf(BOLD"* Writing file...\n"RESET);
		
/* ---- OPEN OUTPUT-FILE ---- */
		
		sprintf(filename, "out-%.03d.ppm", k);
		if (strlen(filename) >= STRINGLENGTH)
		{
			perror(BOLD"\nERROR: output-filename is too long"RESET);
			exit(EXIT_FAILURE);
		}
		
		pFout = fopen(filename, "wb");
		if (pFout == NULL)
		{
			perror(BOLD"\nERROR: fopen: Couldn't open output file"RESET);
			exit(EXIT_FAILURE);
		}
		
/* ---- WRITE OUTPUT FILE ---- */
		
		fprintf(pFout, "P3\n");
		fprintf(pFout, "#Mandelbrot Generator by Sebastian Dichler\n");
		fprintf(pFout, "%u %u\n", width, height);
		fprintf(pFout, "255\n");
		
		for (i = 0; i < height*width; i++)
		{
			fprintf(pFout, "%u %u %u\n", (picture_Pointer_global+i)->r, (picture_Pointer_global+i)->g, (picture_Pointer_global+i)->b);
		}
		
		error = fclose(pFout);
		if (error == EOF)
		{
			perror(BOLD"\nERROR: fclose: Can't close output file"RESET);
			exit(EXIT_FAILURE);
		}
		
		printf(BOLD"* Done writing file!\n"RESET);
		
#if TIME
		gettimeofday(&timer2, NULL);
		
		timediff = (timer2.tv_sec+timer2.tv_usec*0.000001)-(timer1.tv_sec+timer1.tv_usec*0.000001);
		
		printf(BLACK BACKYELLOW"\nWrote file within "BOLDBLACK BACKYELLOW"%f"BLACK BACKYELLOW" secs"RESET"", timediff);
		printf("\n\n");
#endif
		
/* ---- RELEASE ACCESS TO SEMAPHORE 1 ---- */
		
		semaphore1buffer.sem_num = 0;
		semaphore1buffer.sem_op =  1;
		semaphore1buffer.sem_flg = 0;
		
		if (semop(semaphore1, &semaphore1buffer, 1) == -1)
		{
			perror(BOLD"\nERROR: semop: Can't unlock Semaphore 1"RESET);
			exit(EXIT_FAILURE);
		}
		
/* ---- DETACH SHARED MEMORY ---- */
		
		if (shmdt(picture_Pointer_global) == -1)
		{
			perror(BOLD"\nERROR: shmdt: Can't detach Shared-Memory"RESET);
			exit(EXIT_FAILURE);
		}
		
		k++;
	}
	
 	exit(EXIT_SUCCESS);
 }
 
 /*------------------------------------------------------------------*/
/* F U N C T I O N S                                                */
/*------------------------------------------------------------------*/

void cntrl_c_handler_server(int dummy)
{
	printf(BOLD"\nYou just typed CTRL+C\nServer is closing everything...\n"RESET);
	
/* ---- CLOSING SHARED MEMORY ---- */
	
	if (shmctl(sharedmemid, IPC_RMID, NULL) == -1)
	{
		perror(BOLD"\nERROR: shmctl: Can't control Shared-Memory, continue..."RESET);
	}
	
/* ---- CLOSING SEMAPHORE 1 ---- */
	
	if (semctl(semaphore1, IPC_RMID, 0) == -1)
	{
		perror(BOLD"\nERROR: semctl: Can't control Semaphore 1, continue..."RESET);
	}
	
/* ---- CLOSING SEMAPHORE 2 ---- */
	
	if (semctl(semaphore2, IPC_RMID, 0) == -1)
	{
		perror(BOLD"\nERROR: semctl: Can't control Semaphore 2, continue..."RESET);
	}
	
/* ---- CLOSING MESSAGE ---- */
	
	if (msgctl(msqid1, IPC_RMID, 0) == -1)
	{
		perror(BOLD"\nERROR: msgctl: Can't close Message 1, continue..."RESET);
	}
	
	if (msgctl(msqid2, IPC_RMID, 0) == -1)
	{
		perror(BOLD"\nERROR: msctl: Can't close Message 2, continue..."RESET);
	}
	
	printf(BOLD"If any error appeared check it manually with ipcs and remove them with ipcrm\n"RESET);
	
	exit(EXIT_SUCCESS);
}
