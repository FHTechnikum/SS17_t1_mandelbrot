/* !* PIXELGENERATOR PROCESS
 *
 * \description
 *
 *
 * \author Sebastian Dichler <el16b032@technikum-wien.at> <sedi343@gmail.com>
 *
 * \version Rev.: 01, 23.03.2017 - Created
 *          Rev.: 02, 23.03.2017 - created the main function and parts of its declarations
 *          Rev.: 03, 24.03.2017 - Created the init of pixelgen.c and searched web for a algorithm
 *                                 failure present but can't find it.
 *          Rev.: 04, 25.03.2017 - Changed algorithm now its working
 *          Rev.: 05, 25.03.2017 - Created Timestemps and Output file generation in pixelgen.c to check
 *                                 if algorithm is working -> will remove this after programming the writepic.c files
 *
 *
 * \information Algorithm with information of
 *              http://stackoverflow.com/questions/16124127/improvement-to-my-mandelbrot-set-code
 *
 *              bug with shared Mem allocation
 */
 
#include "myhead.h"

struct timeval timer1, timer2, timer3, timer4;
void cntrl_c_handler_client(int dummy);

int main(int argc, char *argv[])
{
	int width = 800;
	int height = 460;
	int iterations = 5000;
	int x, y;
	
	double pr, pi;
	double newRe, oldRe, newIm, oldIm;
	double zoom = 1, moveX = -0.5, moveY = 0;
	
	char widthString[STRINGLENGTH];
	char heightString[STRINGLENGTH];
	char iterationsString[STRINGLENGTH];
	
#if MAKEPIC
	FILE* pFout = NULL;
#endif
	
	int semaphore1;
	int semaphore2;
	int sharedmemid;
	
	PICTURE *picture_Pointer_local = NULL;
	PICTURE *picture_Pointer_shared = NULL;
	
	SEMUN semaphore1union;
	SEMUN semaphore2union;
	SEMBUF semaphore1buffer;
	SEMBUF semaphore2buffer;
	
	key_t keySemaphore;
	key_t keySharedMem;
	
	int i = 0, k = 0, w = 0;
	int error = 0;
	int opt;
	
#if TIME
	double timediff;
#endif
	
	char *pEnd;
	
/*------------------------------------------------------------------*/
/* I N I T                                                          */
/*------------------------------------------------------------------*/
	
	while ((opt = getopt (argc, argv, "w:h:i:z:c:?")) != -1)
	{
		switch (opt)
		{
			case 'w':
				error = clearOptarg(widthString, optarg);
				error = check_number(widthString);
				
				width = strtod(widthString, &pEnd);
			break;
			
			case 'h':
				error = clearOptarg(heightString, optarg);
				error = check_number(heightString);
				
				height = strtod(heightString, &pEnd);
			break;
			
			case 'i':
				error = clearOptarg(iterationsString, optarg);
				error = check_number(iterationsString);
				
				iterations = strtod(iterationsString, &pEnd);
			break;
			
			case 'z':
				clear();
				printf("\nCurrently not supported!\n");
			break;
			
			case 'c':
				clear();
				printf("\nCurrently not supported!\n");
			break;
			
			case '?':
				clearNoHelp();
				helpdesk_2();
				
				exit(EXIT_SUCCESS);
			break;
		}
	}
	
/*------------------------------------------------------------------*/
/* E R R O R   H A N D L I N G                                      */
/*------------------------------------------------------------------*/
	
	clear();
	
/* ---- IF ONE PARAMETER INPUT FAILED OR IS NOT CORRECT ---- */

	if (error == 1)
	{
		printf(BOLD"\nERROR: One or more Parameters are not correct.\n"RESET);
		
		exit(EXIT_FAILURE);
	}
	
/*------------------------------------------------------------------*/
/* I N I T                                                          */
/*------------------------------------------------------------------*/

#if MAKEPIC
	pFout = fopen("out.ppm", "wb");
	if (pFout == NULL)
	{
		perror(BOLD"\nERROR: fopen: couldn't open output file."RESET);
		exit(EXIT_FAILURE);
	}
#endif

/* ---- ALLOCATE MEMORY FOR LOCAL MEMORY ---- */
	
	picture_Pointer_local = (struct picture *)malloc(width * height * sizeof(struct picture));
	if (picture_Pointer_local == NULL)
	{
		perror(BOLD"\nERROR: malloc: Couldn't allocate memory."RESET);
		
		free(picture_Pointer_local);
		exit(EXIT_FAILURE);
	}
	
/* ---- GENERATE KEYS FOR SEMAPHORE AND SHARED MEMORY ---- */
	
	keySemaphore = ftok("/etc/hostname", 'b');
	if (keySemaphore == -1)
	{
		perror(BOLD"ERROR: ftok: can't generate semaphore key"RESET);
		exit(EXIT_FAILURE);
	}
	
	keySharedMem = ftok("/etc/hostname", 'b');
	if (keySharedMem == -1)
	{
		perror(BOLD"ERROR: ftok: can't generate shared mem key"RESET);
		exit (EXIT_FAILURE);
	}
	
/* ---- GENERATE SHARED MEMORY ---- */
	
	sharedmemid = shmget(keySharedMem, width * height * 3, IPC_CREAT | 0666);
	if (sharedmemid < 0)
	{
		perror(BOLD"\nERROR: shmget: Couldn't generate shared memory."RESET);
		
		free(picture_Pointer_local);
		exit(EXIT_FAILURE);
	}
	
/* ---- GENERATE SEMAPHORE 1 ---- */
	
	semaphore1 = semget(keySemaphore, 1, IPC_CREAT | 0666);
	if (semaphore1 < 0)
	{
		perror(BOLD"\nERROR: semget: Couldn't generate semaphore 1"RESET);
		
		free(picture_Pointer_local);	
		exit(EXIT_FAILURE);
	}
	
/* ---- GENERATE SEMAPHORE 2 ---- */
	
	semaphore2 = semget(keySemaphore, 1, IPC_CREAT | 0666);
	if (semaphore2 < 0)
	{
		perror(BOLD"\nERROR: semget: Couldn't generate semaphore 2"RESET);
		
		free(picture_Pointer_local);	
		exit(EXIT_FAILURE);
	}
	
	clear();
	
#if DEBUG
	printf(BOLDRED"Semaphore ID1: %d\n"RESET, semaphore1);
	printf(BOLDRED"Semaphore ID2: %d\n"RESET, semaphore2);
	printf(BOLDRED"Key SharedMem: %d\n"RESET, keySharedMem);
	printf(BOLDRED"Key Semaphore: %d\n"RESET, keySemaphore);
	printf(BOLDRED"Sizeof local Pointer: %d\n"RESET, sizeof(picture_Pointer_local));
	printf(BOLDRED"Sizeof global Pointer: %d\n"RESET, sizeof(picture_Pointer_shared));
	printf(BOLDRED"SIze needed for memory: %d\n"RESET, height*width*3);
	printf(BOLDRED"Size needed for memory: %d\n"RESET, height*width*sizeof(struct picture));
	printf(BOLDRED"Sizeof struct: %d\n"RESET, sizeof(struct picture));
	
	printf(BOLDRED"\nwidth: %d\n"RESET, width);
	printf(BOLDRED"height: %d\n"RESET, height);
	printf(BOLDRED"iterations: %d\n\n"RESET, iterations);
#endif
	
/*------------------------------------------------------------------*/
/* P R O G R A M M   S T A R T                                      */
/*------------------------------------------------------------------*/

/* ---- GENERATE TIME STEMP ---- */

#if TIME
	gettimeofday(&timer1, NULL);
#endif

/* ---- ALGORITHM CODE FOR COLOR (see source in description) ---- */
	
	printf(BOLD"Generating Mandelbrot Pixels...\n"RESET);
	
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pr = 1.5 * (x - width / 2) / (0.5 * zoom * width) + moveX;
			pi = (y - height / 2) / (0.5 * zoom * height) + moveY;
			
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
				
			if (i == iterations)
			{
				(picture_Pointer_local+k)->r = 0;
				(picture_Pointer_local+k)->g = 0;
				(picture_Pointer_local+k)->b = 0;
			}
			else
			{
				double z = sqrt(newRe * newRe + newIm * newIm);
				double iterations_double = iterations;
				int brightness = 256 * log2(1.75 + i - log2(log2(z))) / log2(iterations_double);
				
				(picture_Pointer_local+k)->r = brightness;
				(picture_Pointer_local+k)->g = brightness;
				(picture_Pointer_local+k)->b = 255;
			}
		
		k++;
			
		}
	}
	
	printf(BOLD"Done generating Pixels!\n"RESET);
	
/* ---- GENERATE TIME STAMP ---- */
	
#if TIME
	gettimeofday(&timer2, NULL);
#endif

/* ---- PRINT EVERY PIXEL IN OUTPUT ---- */
	
#if DEBUG
	for (w = 0; w < height*width; w++)
	{
		printf(BOLDRED ITALIC"Pixel: %010d RGB: %03d "RESET, w+1, (picture_Pointer_local+w)->r);
		printf(BOLDRED ITALIC"%03d "RESET, (picture_Pointer_local+w)->g);
		printf(BOLDRED ITALIC"%03d | "RESET, (picture_Pointer_local+w)->b);
	}
	
	printf("\n");
#endif

/* ---- WRITE OUTPUT FILE ---- */

#if MAKEPIC
	printf(BOLDBLACK BACKYELLOW"Writing file..."RESET"\n");
	
	fprintf(pFout, "P3\n");
	fprintf(pFout, "#Mandelbrot Generator by Sebastian Dichler\n");
	fprintf(pFout, "%u %u\n", width, height);
	fprintf(pFout, "255\n");
	
	for (w = 0; w < height*width; w++)
	{
		fprintf(pFout, "%u %u %u\n", (picture_Pointer_local+w)->r, (picture_Pointer_local+w)->g, (picture_Pointer_local+w)->b);
	}
	
	printf(BOLDBLACK BACKYELLOW"Done writing file!"RESET"\n");
#endif
	
#if TIME
	timediff = (timer2.tv_sec+timer2.tv_usec*0.000001)-(timer1.tv_sec+timer1.tv_usec*0.000001);

	printf(BLACK BACKYELLOW"\nGenerated Mandelbrot values within "BOLDBLACK BACKYELLOW"%f"BLACK BACKYELLOW" secs"RESET"\n\n", timediff);
#endif
	
#if MAKEPIC
	error = fclose(pFout);
	if (error == EOF)
	{
		perror(BOLD"\nERROR: fclose: Can't close Outputfile."RESET);
		free(picture_Pointer_local);
		exit(EXIT_FAILURE);
	}
#endif
	free(picture_Pointer_local);
	exit(EXIT_SUCCESS);
}
