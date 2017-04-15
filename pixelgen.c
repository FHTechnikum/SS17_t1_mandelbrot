/* !* PIXELGENERATOR PROCESS
 *
 * \description Generates mandelbrot / apfelmännchien figures with different offset
 *              and zoom
 *
 *
 * \author Sebastian Dichler <el16b032@technikum-wien.at> <sedi343@gmail.com>
 *
 * \version Rev.: 01, 23.03.2017 - Created
 *          Rev.: 02, 23.03.2017 - created the main function and parts of its declarations
 *          Rev.: 03, 24.03.2017 - Created the init of pixelgen.c and searched web for a algorithm
 *                                 failure present but can't find it.
 *          Rev.: 04, 25.03.2017 - Changed algorithm now its working
 *          Rev.: 05, 25.03.2017 - Created Timestemps and Output file generation in pixelgen.c to
 *                                 check if algorithm is working -> will remove this after
 *                                 programming
 *                                 the writepic.c files
 *          Rev.: 06, 26.03.2017 - Added zoom and colorb to parameter (change this to given values
 *                                 with 1,2,3 change)
 *          Rev.: 07, 27.03.2017 - Changed file handling for prototype (algorithm check)
 *          Rev.: 08, 27.03.2017 - Trying mutlithreading, is not working currently
 *          Rev.: 09, 28.03.2017 - Removing manual zoom and move to given values with 1,2,3 change
 *          Rev.: 10, 28.03.2017 - Added switch for given move and zoom values,
 *                                 currently not working right (using zoom)
 *          Rev.: 11, 28.03.2017 - Fixed bug with given move and zoom values, its now working
 *                                 fine :)
 *                                 zoom = 1/zoom
 *          Rev.: 12, 28.03.2017 - Changed templates (colors change is still missing)
 *          Rev.: 13, 28.03.2017 - User output fixes
 *          Rev.: 14, 28.03.2017 - Added template
 *          Rev.: 15, 29.03.2017 - Added another timer and removed the omp library
 *          Rev.: 16, 29.03.2017 - Added colors (not well yet), removed b parameter and added
 *                                 maxrange colorsettigns to algorithm
 *          Rev.: 17, 30.03.2017 - Changed file output
 *          Rev.: 18, 30.03.2017 - Added semaphore1, semaphore2 and sharedmemory to pixelgen
 *                                 -> filling memory with values
 *          Rev.: 19, 30.03.2017 - Communication is working now
 *                                 -> add while(1), SIGNAL, and SIGNAL handler
 *          Rev.: 20, 31.03.2017 - Changed algorithm ratio, its ratio is now correct with
 *                                 width/height instead of 1.5
 *          Rev.: 21, 01.04.2017 - Writing the CNTRL+C handler and while(1) loop
 *          Rev.: 22, 01.04.2017 - Removed Makefile in pixelgen due to working writepic
 *          Rev.: 23, 01.04.2017 - Done but buggy
 *          Rev.: 24, 03.04.2017 - Fix in Useroutput
 *          Rev.: 25, 03.04.2017 - Debug messages for while(1)
 *          Rev.: 26, 04.04.2017 - New semaphore handling (still buggy)
 *          Rev.: 27, 06.04.2017 - Added Message structs and global key and removed global
 *                                 variables
 *          Rev.: 28, 06.04.2017 - Semaphore 2 is allways open?
 *          Rev.: 29, 06.04.2017 - Programms are now working with semaphores, using
 *                                 semget(key,2,...) and setting each semaphores values with
 *                                 sem_num 0 for first and 1 for second semaphore
 *          Rev.: 30, 06.04.2017 - Communication between both programs is now working in a loop 1
 *          Rev.: 31, 06.04.2017 - Reduced global varibales, generate speed in MB/s next to time
 *                                 needed
 *          Rev.: 32, 06.04.2017 - Removed helpdesk at loop beginning
 *          Rev.: 33, 06.04.2017 - The CNTRL+C handler is working but printing error messages
 *          Rev.: 34, 06.04.2017 - Breaks out of while loop if zoom is over 1
 *          Rev.: 35, 07.04.2017 - Added variable zoom out factor for lower zoom in zoomfactor < 0.3
 *          Rev.: 36, 07.04.2017 - Better variable zoom method and better user output
 *          Rev.: 37, 08.04.2017 - Added the getnewzoom function
 *          Rev.: 38, 10.04.2017 - Embellished code and removed some global variables
 *          Rev.: 39, 10.04.2017 - Found a bug in the algorithm
 *          Rev.: 49, 14.04.2017 - Added width and height to global variables because of future
 *                                 cntrl-c handler
 *          Rev.: 50, 15.04.2017 - Changed cntrl-c handler for task
 *                                 pixelgenerator is now closing everything
 *          Rev.: 51, 15.04.2017 - Found bug with global variables and local variables
 *
 *
 * \information Algorithm with information of
 *              http://stackoverflow.com/questions/16124127/improvement-to-my-mandelbrot-set-code
 *              CNTRL+C handler with help of Helmut Resch
 *              Will change color algorithm
 *				Tried to remove global variables requested for the signal handler, but
 *              not working with normal POSIX
 *
 */

#include "myhead.h"

/* ---- GLOBAL VARIABLES ---- */

int width = 800;
int height = 600;
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

	int iterations = 5000;
	double width_double;
	double height_double;
	
	int x, y;
	double pr, pi;
	double newRe, oldRe, newIm, oldIm;
	double zoom, moveX, moveY;
	double currentzoom;
	
	int brightnessr;
	int brightnessg;
	int brightnessb;
	double z;
	double iterations_double;
	int colorr;
	int colorg;
	int colorb;
	
	char widthString[STRINGLENGTH];
	char heightString[STRINGLENGTH];
	char iterationsString[STRINGLENGTH];
	char typeString[STRINGLENGTH];
	
	PICTURE *picture_Pointer_global = NULL;
	
	int i = 0, k = 0, f = 0;
	int error = 0;
	int errorcount = 0;
	int opt;
	int type = 0;
	char *pEnd;
	
#if TIME
	double timediff;
#endif
	
/*------------------------------------------------------------------*/
/* I N I T                                                          */
/*------------------------------------------------------------------*/
	
	clear();
	
/* ---- PARAMETER HANDLING ---- */
	
	while ((opt = getopt (argc, argv, "w:h:i:t:?")) != -1)
	{
		switch (opt)
		{
			case 'w':
				error = clearOptarg(widthString, optarg);
				if (error == 1)
				{
					errorcount++;
				}
				
				error = check_number(widthString);
				if (error == 1)
				{
					errorcount++;
				}
				
				width = strtod(widthString, &pEnd);
			break;
			
			case 'h':
				error = clearOptarg(heightString, optarg);
				if (error == 1)
				{
					errorcount++;
				}
				
				error = check_number(heightString);
				if (error == 1)
				{
					errorcount++;
				}
				
				height = strtod(heightString, &pEnd);
			break;
			
			case 'i':
				error = clearOptarg(iterationsString, optarg);
				if (error == 1)
				{
					errorcount++;
				}
				
				error = check_number(iterationsString);
				if (error == 1)
				{
					errorcount++;
				}
				
				iterations = strtod(iterationsString, &pEnd);
			break;
			
			case 't':
				error = clearOptarg(typeString, optarg);
				if (error == 1)
				{
					errorcount++;
				}
				
				error = check_number(typeString);
				if (error == 1)
				{
					errorcount++;
				}
				
				type = strtod(typeString, &pEnd);
			break;
			
			case '?':
				clearNoHelp();
				helpdesk_2();
				
				exit(EXIT_SUCCESS);
			break;
		}
	}
	
	switch (type)
	{
		case 1:
			moveX = -0.75;
			moveY = 0.1;
			zoom = 0.008;
			
			colorr = 60;
			colorg = -50;
			colorb = -200;
		break;
		
		case 2:
			moveX = -0.7463;
			moveY = 0.1102;
			zoom = 0.005;
			
			colorr = 60;
			colorg = -50;
			colorb = -120;
		break;
		
		case 3:
			moveX = -0.74529;
			moveY = 0.113075;
			zoom = 0.00015;
			
			colorr = 80;
			colorg = 50;
			colorb = 20;
		break;
		
		case 4: /* ---- GOOD TEMPLATE ---- */
			moveX = -0.722;
			moveY = 0.246;
			zoom = 0.019;
			
			colorr = 100;
			colorg = -10;
			colorb = -200;
		break;
		
		case 5: /* ---- CHANGE TEMPLATE / COLOR ALGORITHM ---- */
			moveX = -0.16070135;
			moveY = 1.0375665;
			zoom = 0.00000010;
			
			colorr = -100;
			colorg = -255;
			colorb = -255;
		break;
		
		case 6:
			moveX = -0.0452407411;
			moveY = 0.9868162204352258;
			zoom = 0.00000000013;
			
			colorr = -30;
			colorg = -180;
			colorb = -255;
		break;
		
		case 7: /* ---- CHANGE TEMPLATE / COLOR ALGORITHM ---- */
			moveX = 0.281717921930775;
			moveY = 0.5771052841488505;
			zoom = 0.0000000000000192;
			
			colorr = -80;
			colorg = -140;
			colorb = -140;
		break;
		
		case 8:
			moveX = 0.432539867562512;
			moveY = 0.226118675951765;
			zoom = 0.0000032;
			
			colorr = 30;
			colorg = -50;
			colorb = -200;
		break;
		
		case 9:
			moveX = -1.99999911758838;
			moveY = 0;
			zoom = 0.00000000000128;
			
			colorr = 100;
			colorg = -40;
			colorb = -200;
		break;
		
		case 10:
			moveX = -1.296354375872899;
			moveY = 0.44185155566229;
			zoom = 0.0000000000006;
			
			colorr = 10;
			colorg = -100;
			colorb = -200;
		break;
		
		default:
			moveX = -0.5;
			moveY = 0;
			zoom = 1;
			
			colorr = 30;
			colorg = -80;
			colorb = 80;
		break;
	}
	
/*------------------------------------------------------------------*/
/* E R R O R   H A N D L I N G                                      */
/*------------------------------------------------------------------*/
	
/* ---- IF ONE PARAMETER INPUT FAILED OR IS NOT CORRECT ---- */
	
	if (errorcount != 0)
	{
		perror(BOLD"\nERROR: One or more Parameters are not correct"RESET);
		exit(EXIT_FAILURE);
	}
	
	if (type < 0 || type > 10)
	{
		perror(BOLD"\nERROR: Type value must be between 0 and 10"RESET);
		exit(EXIT_FAILURE);
	}
	
/*------------------------------------------------------------------*/
/* I N I T                                                          */
/*------------------------------------------------------------------*/
	
/* ---- GENERATE KEYS FOR SEMAPHORE, SHARED MEMORY AND MESSAGE ---- */
	
	globalKey = getkey();
	
/* ---- GENERATE SEMAPHORE 1 AND 2 ---- */
	
	semaphore = semget(globalKey, 2, IPC_CREAT | 0666);
	if (semaphore < 0)
	{
		semaphore = semget(globalKey, 2, 0);
		if (semaphore < 0)
		{
			perror(BOLD"\nERROR: semget: Couldn't generate Semaphore"RESET);
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
	
/* ---- GENERATE SHARED MEMORY ---- */
	
	sharedmemid = shmget(globalKey, (width * height * sizeof(PICTURE)), IPC_CREAT | 0666);
	if (sharedmemid == -1 || errno == ENOENT)
	{
		perror(BOLD"\nERROR: shmget: Couldn't generate Shared-Memory"RESET);
		exit(EXIT_FAILURE);
	}
	
/* ---- ALLOCATE MEMORY FOR LOCAL MEMORY ---- */
	
	picture_Pointer_local = (PICTURE *)malloc(width * height * sizeof(PICTURE));
	if (picture_Pointer_local == NULL)
	{
		perror(BOLD"\nERROR: malloc: Couldn't allocate local memory"RESET);
		free(picture_Pointer_local);
		exit(EXIT_FAILURE);
	}
	
/* ---- MESSAGE SEND ---- */
	
	messagetype.mtype = 0;
	messagetype.msg.width = width;
	messagetype.msg.height = height;
	messagetype.msg.type = type;
	
	typeMessage = msgget(globalKey, IPC_CREAT | 0666);
	if (typeMessage >= 0)
	{
		if (msgsnd(typeMessage, &messagetype.msg, sizeof(messagetype.msg), 0) == -1)
		{
			perror(BOLD"\nERROR: msgsnd: Can't send message"RESET);
			free(picture_Pointer_local);
			exit(EXIT_FAILURE);
		}
	}
	
#if DEBUG
	printf(BOLDRED"Semaphore ID: %d\n"RESET, semaphore);
	printf(BOLDRED"Sharedmem ID: %d\n"RESET, sharedmemid);
	printf(BOLDRED"Message ID: %ld\n"RESET, typeMessage);;
	printf(BOLDRED"Key: %d\n\n"RESET, globalKey);
	printf(BOLDRED"Size for each picture: %dkByte\n"RESET, (sizeof(PICTURE)*height*width)/1000);
	
	printf(BOLDRED"\nwidth: %d\n"RESET, width);
	printf(BOLDRED"height: %d\n"RESET, height);
	printf(BOLDRED"iterations: %d\n"RESET, iterations);
	printf(BOLDRED"Type: %d\n", type);
	printf(BOLDRED"-moveX: %.15f -moveY: %.15f -zoom: %.15f\n\n"RESET, moveX, moveY, zoom);
#endif
	
/*------------------------------------------------------------------*/
/* P R O G R A M M   S T A R T                                      */
/*------------------------------------------------------------------*/
	
/* ---- USER OUTPUT ---- */
	
	printf(BOLD ITALIC"\n*** GENERATING MANDELBROT ***\n\n"RESET);
	
/* ---- NEEDED FOR ALGORITHM BUT ARE STATIC ----*/
	
	width_double = width;
	height_double = height;
	currentzoom = zoom;
	
/* ---- CTRL+C HANDLER ---- */
	
	signal(SIGINT, cntrl_c_handler_pixelgen);
	
/* ---- ALGORITHM START ---- */
	
	while(1)
	{
		if (f > 0)
		{
			printf(ITALIC"_________________________________________\n\n"RESET);
			printf(BACKGREEN BLACK"New Image:"RESET"\n\n");
		}
		
		sleep(1);
		
/* ---- ALGORITHM CODE FOR COLOR (source in description (algorithm changed!)) ---- */
		
#if TIME
		gettimeofday(&timer1, NULL);
#endif
		
		printf("* Generating Mandelbrot Pixels...\n");
		
		k = 0;
		
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				pr = (width_double/height_double) * (x - width / 2) / (0.5 * (1/currentzoom) * width) + moveX;
				pi = (y - height / 2) / (0.5 * (1/currentzoom) * height) + moveY;
				
				newRe = newIm = oldRe = oldIm = 0;
				
				for (i = 0; i < iterations; i++)
				{
					oldRe = newRe;
					oldIm = newIm;
					
					newRe = oldRe * oldRe - oldIm * oldIm + pr;
					newIm = 2 * oldRe * oldIm + pi;
					
					if ((newRe * newRe + newIm * newIm) > 4)
					{
						break;
					}
				}
				
/* ---- WRITE BLACK PIXELS ---- */
				
				if (i == iterations)
				{
					(picture_Pointer_local+k)->r = 0;
					(picture_Pointer_local+k)->g = 0;
					(picture_Pointer_local+k)->b = 0;
				}
				
/* ---- WRITE COLORED PIXELS ---- */
				
				else
				{
					z = sqrt(newRe * newRe + newIm * newIm);
					iterations_double = iterations;
					brightnessr = 256 * log2(1.75 + i - log2(log2(z))) / log2(iterations_double) + colorr;
					brightnessg = 256 * log2(1.75 + i - log2(log2(z))) / log2(iterations_double) + colorg;
					brightnessb = 256 * log2(1.75 + i - log2(log2(z))) / log2(iterations_double) + colorb;
					
					(picture_Pointer_local+k)->r = brightnessr;
					(picture_Pointer_local+k)->g = brightnessg;
					(picture_Pointer_local+k)->b = brightnessb;
					
/* ---- COLORS ARE LOWER THAN 0 ---- */
					
					if ((picture_Pointer_local+k)->r < 0)
					{
						(picture_Pointer_local+k)->r = 0;
					}
					if ((picture_Pointer_local+k)->g < 0)
					{
						(picture_Pointer_local+k)->g = 0;
					}
					if ((picture_Pointer_local+k)->b < 0)
					{
						(picture_Pointer_local+k)->b = 0;
					}
					
/* ---- COLORS ARE HIGHER THAN 255 ---- */
					
					if ((picture_Pointer_local+k)->r > 255)
					{
						(picture_Pointer_local+k)->r = 255;
					}
					if ((picture_Pointer_local+k)->g > 255)
					{
						(picture_Pointer_local+k)->g = 255;
					}
					if ((picture_Pointer_local+k)->b > 255)
					{
						(picture_Pointer_local+k)->b = 255;
					}
				}
				
				k++;
			}
		}
		
		currentzoom = getnewzoom(currentzoom);
		
		printf("* Done generating Pixels!\n\n");
		printf("Generated Pixels for "BOLD ITALIC"\"picture-%.03d.ppm\"\n\n"RESET, f);
		
		f++;
		
		printf(ITALIC"Values:\n"RESET);
		printf("-Zoom: %.07f -OffsetX: %f -OffsetY: %f\n\n", currentzoom, moveX, moveY);
		
#if TIME
		gettimeofday(&timer2, NULL);
#endif
		
/* ---- PRINT EVERY PIXEL IN OUTPUT ---- */
		
#if DEBUG_PIXEL
		for (w = 0; w < height*width; w++)
		{
			printf(BOLDRED ITALIC"Pixel: %010d RGB: %03d "RESET, w+1, (picture_Pointer_local+w)->r);
			printf(BOLDRED ITALIC"%03d "RESET, (picture_Pointer_local+w)->g);
			printf(BOLDRED ITALIC"%03d | "RESET, (picture_Pointer_local+w)->b);
		}
		
		printf("\n");
#endif
		
/* ---- ATTACH SHARED MEMORY ---- */
		
#if DEBUG
		printf(BOLDRED"Attaching Shared-Memory\n"RESET);
#endif
		
		picture_Pointer_global = shmat(sharedmemid, 0, 0);
		if (picture_Pointer_global == (PICTURE *)-1)
		{
			perror(BOLD"\nERROR: shamat: Can't attach Shared-Memory"RESET);
			free(picture_Pointer_local);
			exit(EXIT_FAILURE);
		}
		
#if DEBUG
		printf(BOLDRED"Attached Shared-Memory\n"RESET);
#endif
		
/* ---- REQUEST ACCESS TO SEMAPHORE 1 ---- */
		
#if DEBUG
		printf(BOLDRED"Requesting access to Semaphore 1\n"RESET);
#endif
		
		semaphore1buffer.sem_num = 0;
		semaphore1buffer.sem_op = -1;
		semaphore1buffer.sem_flg = 0;
		
		if (semop(semaphore, &semaphore1buffer, 1) == -1)
		{
			perror(BOLD"\nERROR: semop: Can't access Semaphore 1"RESET);
			free(picture_Pointer_local);
			exit(EXIT_FAILURE);
		}
		
#if DEBUG
		printf(BOLDRED"Have access to Semaphore 1\n"RESET);
#endif
		
/* ---- FILL SHARED MEMORY WITH LOCAL POINTER VALUES ---- */
		
#if DEBUG
		printf(BOLDRED"Filling Shared-Memory with values\n"RESET);
#endif
		
		for (i = 0; i < width*height; i++)
		{
			(picture_Pointer_global+i)->r = (picture_Pointer_local+i)->r;
			(picture_Pointer_global+i)->g = (picture_Pointer_local+i)->g;
			(picture_Pointer_global+i)->b = (picture_Pointer_local+i)->b;
		}
		
#if DEBUG
		printf(BOLDRED"Filled Shared-Memory with values\n"RESET);
#endif
		
/* ---- RELEASE ACCESS TO SEMAPHORE 2 ---- */
		
#if DEBUG
		printf(BOLDRED"Release access to Semaphore 2\n"RESET);
#endif
		
		semaphore2buffer.sem_num = 1;
		semaphore2buffer.sem_op =  1;
		semaphore2buffer.sem_flg = 0;
		
		if (semop(semaphore, &semaphore2buffer, 1) == -1)
		{
			perror(BOLD"\nERROR: semop: Can't release access to Semaphore 2"RESET);
			free(picture_Pointer_local);
			exit(EXIT_FAILURE);
		}
		
#if DEBUG
		printf(BOLDRED"Released access to Semaphore 2\n"RESET);
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
		
#if TIME
		timediff = (timer2.tv_sec+timer2.tv_usec*0.000001)-(timer1.tv_sec+timer1.tv_usec*0.000001);
		
		printf("\n"BLACK BACKYELLOW"Generated pixels within "BOLDBLACK BACKYELLOW"%f"BLACK BACKYELLOW" secs"RESET"\n", timediff);
		printf(BLACK BACKYELLOW"Generate Speed: "BOLDBLACK BACKYELLOW"%.2fMB/s"RESET"\n\n", (((sizeof(PICTURE)*height*width)/1000000)/timediff));
#endif
		
	}
	
	free(picture_Pointer_local);
	exit(EXIT_SUCCESS);
}

/*------------------------------------------------------------------*/
/* F U N C T I O N S                                                */
/*------------------------------------------------------------------*/

void cntrl_c_handler_pixelgen(int dummy)
{
	key_t globalKey;
	
	long int typeMessage = 0;
	
	int semaphore = 0;
	int sharedmemid = 0;
	
	globalKey = getkey();
	
	printf(BOLD"\nYou just typed CTRL+C\nPixelgenerator is closing everything...\n\n"RESET);
	
/* ---- CLOSING SHARED MEMORY ---- */
	
	sharedmemid = shmget(globalKey, (width * height * sizeof(PICTURE)), IPC_CREAT | 0666);
	if (sharedmemid == -1)
	{
		perror(BOLD"\nERROR: shmget: Couldn't generate Shared-Memory."RESET);
	}
	
	if (shmctl(sharedmemid, IPC_RMID, NULL) == -1)
	{
		perror(BOLD"\nERROR: shmctl: Can't control Shared-Memory, continue..."RESET);
	}
	
/* ---- CLOSING SEMAPHORE ---- */
	
	semaphore = semget(globalKey, 2, IPC_CREAT | 0666);
	if (semaphore < 0)
	{
		semaphore = semget(globalKey, 2, 0);
		if (semaphore < 0)
		{
			perror(BOLD"\nERROR: semget: Couldn't generate Semaphore"RESET);
		}
	}
	
	if (semctl(semaphore, IPC_RMID, 0) == -1)
	{
		perror(BOLD"\nERROR: semctl: Can't control Semaphore 1, continue..."RESET);
	}
	
/* ---- CLOSING MESSAGE ---- */
	
	typeMessage = msgget(globalKey, IPC_CREAT | 0666);
	if (typeMessage == -1)
	{
		perror(BOLD"\nERROR: msgget: Can't get Message"RESET);
	}
	
	if (msgctl(typeMessage, IPC_RMID, 0) == -1)
	{
		perror(BOLD"\nERROR: msgctl: Can't close Message, continue..."RESET);
	}
	
	free(picture_Pointer_local);
	exit(EXIT_SUCCESS);
}
