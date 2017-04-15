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
 *          Rev.: 06, 03.04.2017 - Debug messages for while(1)
 *          Rev.: 07, 04.04.2017 - New semaphore handling (still buggy)
 *          Rev.: 08, 06.04.2017 - Added Message structs and global key and removed global
 *                                 varibales
 *          Rev.: 09, 06.04.2017 - Semaphore 2 is allways open?
 *          Rev.: 10, 06.04.2017 - Programms are now working with semaphores, using
 *                                 semget(key,2,...) and setting each semaphores values with
 *                                 sem_num 0 for first and 1 for second semaphore
 *          Rev.: 11, 06.04.2017 - Communication between both programs is now working in a loop 1
 *          Rev.: 12, 06.04.2017 - Reduced global varibales, write speed in MB/s next to time needed
 *          Rev.: 13, 06.04.2017 - Removed helpdesk at loop beginning
 *          Rev.: 14, 06.04.2017 - The CNTRL+C handler is working but printing error messages
 *          Rev.: 15, 07.04.2017 - Better variable zoom method and better user output
 *          Rev.: 16, 08.04.2017 - Better user output
 *          Rev.: 17, 10.04.2017 - Embellished code and removed some global variables
 *          Rev.: 18, 14.04.2017 - Added a local memory due to task description
 *          Rev.: 19, 14.04.2017 - Changed structure of code due to task description
 *          Rev.: 20, 14.04.2017 - Bug fix in cntrl-c handler
 *          Rev.: 21, 15.04.2017 - Changed cntrl-c handler for task
 *                                 pixelgenerator is now closing everything
 *          Rev.: 22, 15.04.2017 - Writepic only checks if memory and semaphores are present
 *
 *
 * \information CNTRL+C handler with help of Helmut Resch
 *              Tried to remove global variables requested for the signal handler, but
 *              not working with normal POSIX
 *
 *
 */

#include "myhead.h"

/* ---- GLOBAL VARIABLES ---- */

PICTURE *picture_Pointer_local = NULL;

/* ---- MAIN FUNCTION ---- */

int main(int argc, char *argv[])
{
	
#if TIME 
	struct timeval timer1, timer2;
#endif
	
	key_t globalKey;
	
	SEMUN semaphore1union;
	SEMUN semaphore2union;
	SEMBUF semaphore1buffer;
	SEMBUF semaphore2buffer;
	int semaphore = 0;
	int sharedmemid = 0;
	
	MESSAGE messagetype;
	long int typeMessage = 0;
	long int mtype = 0;

	int type;
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
	
/* ---- GENERATE KEYS FOR SEMAPHORE, SHARED MEMORY AND MESSAGE ---- */
	
	globalKey = getkey();
	
/* ---- MESSAGE RCV ---- */
	
	typeMessage = msgget(globalKey, IPC_CREAT | 0666);
	if (typeMessage >= 0)
	{
		if (msgrcv(typeMessage, &messagetype.msg, sizeof(messagetype.msg), mtype, 0) == -1)
		{
			perror(BOLD"\nERROR: msgsnd: Can't read width"RESET);
			exit(EXIT_FAILURE);
		}
	}
	
	mtype = 0;
	width = messagetype.msg.width;
	height = messagetype.msg.height;
	type = messagetype.msg.type;
	
/* ---- GENERATE SHARED MEMORY ---- */
	
	sharedmemid = shmget(globalKey, (width * height * sizeof(PICTURE)), 0);
	if (sharedmemid == -1 || errno == ENOENT)
	{
		perror(BOLD"\nERROR: shmget: Couldn't find Shared-Memory."RESET);
		exit(EXIT_FAILURE);
	}
	
/* ---- GENERATE SEMAPHORE 1 AND 2 ---- */
	
	semaphore = semget(globalKey, 2, 0);
	if (semaphore < 0)
	{
		semaphore = semget(globalKey, 2, 0);
		if (semaphore < 0)
		{
			perror(BOLD"\nERROR: semget: Couldn't find Semaphore"RESET);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
	
/* ---- SEMAPHORE 1 | OPEN | KEY IS AVAILABLE ---- */
	
		semaphore1union.val = 1;
	
		if (semctl(semaphore, 0, SETVAL, semaphore1union) < 0)
		{
			perror(BOLD"\nERROR: semctl: Can't control Semaphore 1"RESET);
			exit(EXIT_FAILURE);
		}
		
/* ---- SEMAPHORE 2 | CLOSED | KEY IS NOT AVAILABLE ---- */
		
		semaphore2union.val = 0;
	
		if (semctl(semaphore, 1, SETVAL, semaphore2union) < 0)
		{
			perror(BOLD"\nERROR: semctl: Can't control Semaphore 2"RESET);
			exit(EXIT_FAILURE);
		}
	}
	
/* ---- ALLOCATE MEMORY FOR LOCAL MEMORY ---- */
	
	picture_Pointer_local = (PICTURE *)malloc(width * height * sizeof(PICTURE));
	if (picture_Pointer_local == NULL)
	{
		perror(BOLD"\nERROR: malloc: Couldn't allocate local memory"RESET);
		free(picture_Pointer_local);
		exit(EXIT_FAILURE);
	}
		
	clear();
	
#if DEBUG
	printf(BOLDRED"Semaphore ID: %d\n"RESET, semaphore);
	printf(BOLDRED"Sharedmem ID: %d\n"RESET, sharedmemid);
	printf(BOLDRED"Message ID: %ld\n"RESET, typeMessage);
	printf(BOLDRED"Key: %d\n\n"RESET, globalKey);
	printf(BOLDRED"Size for each picture: %dkByte\n"RESET, (sizeof(PICTURE)*height*width)/1000);
	
	printf(BOLDRED"Width: %d\n"RESET, width);
	printf(BOLDRED"Height: %d\n"RESET, height);
	printf(BOLDRED"Type: %d\n\n"RESET, type);
#endif
	
/*------------------------------------------------------------------*/
/* P R O G R A M M   S T A R T                                      */
/*------------------------------------------------------------------*/
	
/* ---- USER OUTPUT ---- */
	
	printf(BOLD ITALIC"*** Waiting for data from client... ***\n\n"RESET);

/* ---- CTRL+C HANDLER ---- */
	
	signal(SIGINT, cntrl_c_handler_writepic);
	
/* ---- ALGORITHM START ---- */
	
	while(1)
	{
		if (k > 0)
		{
			printf(ITALIC"_________________________________________\n\n"RESET);
			printf(BACKGREEN BLACK"New Image:"RESET"\n\n");
		}
		
		printf("Waiting for data...\n\n");
		
/* ---- ATTACH SHARED MEMORY ---- */
		
#if DEBUG
		printf(BOLDRED"Attaching Shared-Memory\n"RESET);
#endif
		
		picture_Pointer_global = shmat(sharedmemid, 0, 0);
		if (picture_Pointer_global == (PICTURE *)-1)
		{
			perror(BOLD"ERROR: shamt: Can't attach Shared-Memory"RESET);
			free(picture_Pointer_local);
			exit(EXIT_FAILURE);
		}
		
#if DEBUG
		printf(BOLDRED"Attached Shared-Memory\n"RESET);
#endif
		
/* ---- REQUEST ACCESS TO SEMAPHORE 2 ---- */
		
#if DEBUG
		printf(BOLDRED"Requesting access to Semaphore 2\n"RESET);
#endif
		
		semaphore2buffer.sem_num = 1;
		semaphore2buffer.sem_op = -1;
		semaphore2buffer.sem_flg = 0;
		
		if (semop(semaphore, &semaphore2buffer, 1) == -1)
		{
			perror(BOLD"\nERROR: semop: Can't unlock Semaphore 2"RESET);
			free(picture_Pointer_local);
			exit(EXIT_FAILURE);
		}
		
#if DEBUG
		printf(BOLDRED"Have access to Semaphore 2\n"RESET);
#endif
		
/* ---- WRITING GLOBAL MEMORY TO LOCAL MEMORY ---- */
		
		for (i = 0; i < height*width; i++)
		{
			(picture_Pointer_local+i)->r = (picture_Pointer_global+i)->r;
			(picture_Pointer_local+i)->g = (picture_Pointer_global+i)->g;
			(picture_Pointer_local+i)->b = (picture_Pointer_global+i)->b;
		}
		
/* ---- RELEASE ACCESS TO SEMAPHORE 1 ---- */
		
#if DEBUG
		printf(BOLDRED"Release access to Semaphore 1\n"RESET);
#endif
		
		semaphore1buffer.sem_num = 0;
		semaphore1buffer.sem_op =  1;
		semaphore1buffer.sem_flg = 0;
		
		if (semop(semaphore, &semaphore1buffer, 1) == -1)
		{
			perror(BOLD"\nERROR: semop: Can't unlock Semaphore 1"RESET);
			perror(BOLD"Pixelgenerator maybe closed session!"RESET);
			free(picture_Pointer_local);
			exit(EXIT_FAILURE);
		}
		
#if DEBUG
		printf(BOLDRED"Released access to Semaphore 1\n"RESET);
#endif
		
/* ---- DETACH SHARED MEMORY ---- */
		
#if DEBUG
		printf(BOLDRED"Detach Shared-Memory\n"RESET);
#endif
		
		if (shmdt(picture_Pointer_global) == -1)
		{
			perror(BOLD"\nERROR: shmdt: Can't detach Shared-Memory"RESET);
			free(picture_Pointer_local);
			exit(EXIT_FAILURE);
		}
		
#if DEBUG
		printf(BOLDRED"Detached Shared-Memory\n"RESET);
#endif

/* ---- WRITE FILE ---- */
		
#if TIME
		gettimeofday(&timer1, NULL);
#endif
		
		printf("* Writing file...\n");
		
/* ---- OPEN OUTPUT-FILE ---- */
		
		sprintf(filename, "picture-%.03d.ppm", k);
		if (k > 999)
		{
			perror(BOLD"\nERROR: reached maximum pictures value of 1000"RESET);
			free(picture_Pointer_local);
			exit(EXIT_FAILURE);
		}
		
		pFout = fopen(filename, "wb");
		if (pFout == NULL)
		{
			perror(BOLD"\nERROR: fopen: Couldn't open output file"RESET);
			free(picture_Pointer_local);
			exit(EXIT_FAILURE);
		}
		
/* ---- WRITE OUTPUT FILE ---- */
		
		fprintf(pFout, "P3\n");
		fprintf(pFout, "#Mandelbrot Generator by Sebastian Dichler\n");
		fprintf(pFout, "#Tpye: %d\n", type);
		fprintf(pFout, "%u %u\n", width, height);
		fprintf(pFout, "255\n");
		
		for (i = 0; i < height*width; i++)
		{
			fprintf(pFout, "%u %u %u\n", (picture_Pointer_local+i)->r, (picture_Pointer_local+i)->g, (picture_Pointer_local+i)->b);
		}
		
		error = fclose(pFout);
		if (error == EOF)
		{
			perror(BOLD"\nERROR: fclose: Can't close output file"RESET);
			free(picture_Pointer_local);
			exit(EXIT_FAILURE);
		}
		
		printf("* Done writing file!\n\n");
		printf("Generated Picture "BOLD ITALIC"\"picture-%.03d.ppm\"\n"RESET, k);
		
#if TIME
		gettimeofday(&timer2, NULL);
		
		timediff = (timer2.tv_sec+timer2.tv_usec*0.000001)-(timer1.tv_sec+timer1.tv_usec*0.000001);
		
		printf("\n"BLACK BACKYELLOW"Wrote file within "BOLDBLACK BACKYELLOW"%f"BLACK BACKYELLOW" secs"RESET"\n", timediff);
		printf(BLACK BACKYELLOW"Write Speed: "BOLDBLACK BACKYELLOW"%.2fMB/s"RESET"\n\n", (((sizeof(PICTURE)*height*width)/1000000)/timediff));
#endif
		
		k++;
	}
	
	free(picture_Pointer_local);
 	exit(EXIT_SUCCESS);
 }
 
 /*------------------------------------------------------------------*/
/* F U N C T I O N S                                                */
/*------------------------------------------------------------------*/

void cntrl_c_handler_writepic(int dummy)
{
	key_t globalKey;
	
	SEMBUF semaphore1buffer;
	int semaphore = 0;
	
	globalKey = getkey();
	
	semaphore = semget(globalKey, 2, IPC_CREAT | 0666);
	if (semaphore < 0)
	{
		semaphore = semget(globalKey, 2, 0);
		if (semaphore < 0)
		{
			perror(BOLD"\nERROR: semget: Couldn't generate Semaphore"RESET);
			free(picture_Pointer_local);
			exit(EXIT_FAILURE);
		}
	}
	
	printf(BOLD"\nYou just typed CTRL+C\nWriter is closing connection...\n\n"RESET);
	
/* ---- REQUESTING ACCESS TO SEMAPHORE 1 ---- */
	
	semaphore1buffer.sem_num = 0;
	semaphore1buffer.sem_op = -1;
	semaphore1buffer.sem_flg = IPC_NOWAIT;
	
	if (semop(semaphore, &semaphore1buffer, 1) == -1)
	{
		perror(BOLD"\nERROR: semop: Can't lock Semaphore 1"RESET);
	}
	
	free(picture_Pointer_local);
	exit(EXIT_SUCCESS);
}
