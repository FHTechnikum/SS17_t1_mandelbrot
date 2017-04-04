/* !* HEADER FILE FOR MANDELBROT TASK
 *
 * \description
 *
 *
 * \author Sebastian Dichler <el16b032@technikum-wien.at> <sedi343@gmail.com>
 *
 * \version Rev.: 01, 23.03.2017 - Created
 *          Rev.: 02, 25.03.2017 - Added <sys/time.h> library for timestemps
 *          Rev.: 03, 25.03.2017 - Added some ANSI formats for output
 *          Rev.: 04, 28.03.2017 - Added debug_pixel define
 *          Rev.: 05, 28.03.2017 - Fixed ifndef failure
 *
 * \information A bug with ifndef and define of _myhead_, can't find it
 *
 */

#ifndef _myhead_
#define _myhead_

/* ---- SYSTEM BASED LIBRARYS ---- */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>

/* ---- STANDARD LIBRARYS ---- */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <ctype.h>


/* ---- ANSI FORMAT FOR TERMINAL ---- */

#define RESET         "\033[0m"
#define BLACK         "\033[30m"             /* Black */
#define RED           "\033[31m"             /* Red */
#define GREEN         "\033[32m"             /* Green */
#define CYAN          "\033[36m"             /* Cyan */

#define BOLD          "\033[1m"              /* Bold */
#define BOLDRED       "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN     "\033[1m\033[32m"      /* Bold Green */
#define BOLDBLACK     "\033[1m\033[30m"      /* Bold Black */
#define BOLDCYAN      "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE     "\033[1m\033[37m"      /* Bold White */

#define BACKBLACK     "\033[100m"            /* Background Black */
#define BACKRED       "\033[101m"            /* Background Red */
#define BACKGREEN     "\033[102m"            /* Background Green */
#define BACKYELLOW    "\033[103m"            /* Background Yellow */
#define BACKBLUE      "\033[104m"            /* Background Blue */
#define BACKMAGENTA   "\033[105m"            /* Background Magenta */
#define BACKCYAN      "\033[106m"            /* Background Cyan */
#define BACKWHITE     "\033[107m"            /* Background White */

#define ITALIC        "\033[3m"              /* Italic Mode */


#define DEBUG 1
#define DEBUG_PIXEL 0
#define TIME 1

#define STRINGLENGTH 500

struct picture
{
	int r;
	int g;
	int b;
};

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};

typedef union semun SEMUN;
typedef struct sembuf SEMBUF;
typedef struct picture PICTURE;

void clear(void);
void clearNoHelp(void);
void helpdesk_1(void);
void helpdesk_2(void);
int check_number(char *number);
int clearOptarg(char *string, char *input);

void cntrl_c_handler_client(int dummy);
void cntrl_c_handler_server(int dummy);

#endif /* _myhead_ */
