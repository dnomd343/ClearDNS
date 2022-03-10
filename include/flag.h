#ifndef _EXIT_CODE_H_
#define _EXIT_CODE_H_

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define EXIT_NORMAL 0
#define EXIT_FILE_ERROR 1
#define EXIT_FORK_ERROR 2
#define EXIT_EXEC_ERROR 3
#define EXIT_WAIT_ERROR 4

#endif
