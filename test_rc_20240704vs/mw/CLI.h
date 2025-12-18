/******************** (C) COPYRIGHT MEMSPlus ********************
* File Name          : CLI.h
* Author             : Jackie
* Version            : V1.0
* Date               : 02/10/2016
* Description        : Simple Command Line Interface  (CLI)
*******************************************************************************/

#ifndef __CLI_H__
#define __CLI_H__

#include "libcfg.h"

#ifndef CLI_CMD_USER_DEFINE
#define CLI_CMD_USER_DEFINE
#endif /* !CLI_CMD_USER_DEFINE */

#ifndef HW_VER
#error  "miss hardware info !"
#endif /* !HW_VER */

#ifndef SW_VER
#define SW_VER					"00"
#endif /* !HW_VER */

#define VER_TIME_STAMP			__DATE__,__TIME__
#define VENDOR				"COPYRIGHT SINETEQ"

#ifndef CLI_CMD_END_CHAR
#define CLI_CMD_END_CHAR		0x0D
#endif /* !CLI_CMD_END_CHAR */

#ifndef CLI_DELIM_STR
#define CLI_DELIM_STR			" "
#endif /* !CLI_DELIM_STR */

#ifndef CLI_CMD_BUF_LEN
#define CLI_CMD_BUF_LEN			1000
#endif /* !CLI_CMD_BUF_LEN */

#ifndef CLI_ARGS_NUM_MAX
#define CLI_ARGS_NUM_MAX		0
#endif /* !CLI_ARGS_NUM_MAX */

/* callback function */
typedef s8 (*CmdHandler_t)(u8, char**);

/* define command line: <command> <param1> <param2> ... <paramN> */
typedef struct cmdLineEntry_s 
{
  char          *cmd;
  u8            param_num;
  CmdHandler_t 	cmd_handler;
  u8            lvl;
}cmdLineEntry_t;

typedef struct _CLIEntryList_s
{
  cmdLineEntry_t                *entry;
  struct _CLIEntryList_s        *next;
} CLIEntryList_t;

#ifdef CLI_ENABLE
    #define __CLI_INIT()    CLI_Init()
    #define __CLI_ENTRY(name, cmdLine, argNum)				\
            cmdLineEntry_t name =                                           \
            {								\
                .cmd = cmdLine,						\
                .param_num = ((argNum > CLI_ARGS_NUM_MAX) ?                 \
                              CLI_ARGS_NUM_MAX : argNum),			\
                .cmd_handler = CLI_CmdHandler_##name,		        \
                .lvl = 0                                                    \
            }
    #define __CLI_TOP_ENTRY(name, cmdLine, argNum)				\
            cmdLineEntry_t name =                                           \
            {								\
                .cmd = cmdLine,						\
                .param_num = ((argNum > CLI_ARGS_NUM_MAX) ?                 \
                              CLI_ARGS_NUM_MAX : argNum),			\
              .cmd_handler = CLI_CmdHandler_##name,		                \
                .lvl = 1                                                    \
            }
    #define __CLI_ADDENTRY(name)	CLI_AddEntry(&name)
#else
    #define __CLI_INIT()
    #define __CLI_ENTRY(name, cmdLine, argNum)
    #define __CLI_TOP_ENTRY(name, cmdLine, argNum)
    #define __CLI_ADDENTRY(name)
#endif

void CLI_Init(void);
int CLI_AddEntry(cmdLineEntry_t *entry);
void CLI_RevCB(u8 Chr);

#endif /* !__CLI_H__ */