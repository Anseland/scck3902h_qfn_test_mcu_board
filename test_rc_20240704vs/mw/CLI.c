/******************** (C) COPYRIGHT MEMSPlus ********************
* File Name          : CLI.c
* Author             : Jackie
* Version            : V1.0
* Date               : 02/10/2016
* Description        : Simple Command Line Interface
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CLI.h"

#define COUNT(a) (sizeof(a) / sizeof(a[0]))

static CLIEntryList_t *CLIListHead = NULL;
static char CmdBuf[CLI_CMD_BUF_LEN] = {0};
static u8 EchoState = 1;  /* 1;*/
const char *PROMPT = "----cli----\n";

void CLI_Prompt(void)
{
    if(EchoState)
    {
        printf(PROMPT);
    }
}

s8 CLI_CmdHandler_ShowVersion(u8 argc, char *argv[])
{
	printf("\n%s %s \n%s,%s\n%s\n", HW_VER, SW_VER, VER_TIME_STAMP, VENDOR);
	return 0;
}

s8 CLI_CmdHandler_Echo(u8 argc, char *argv[])
{
	if(argc != 1){
        return -1;
    }

    if( !strcmp("ON", argv[0]) ){
        if( !EchoState ){
            EchoState = 1;
        }
        printf("\nEcho on.\n");
    }
    else if( !strcmp("OFF", argv[0]) ){
        if( EchoState ){
            EchoState = 0;
        }
        printf("\nEcho off.\n");
    }
	return 0;
}

__CLI_TOP_ENTRY(ShowVersion,    "M.VER",    0);
__CLI_TOP_ENTRY(Echo,           "ECHO",     1);

int CLI_AddEntry(cmdLineEntry_t *entry)
{
	CLIEntryList_t *pEntry = CLIListHead;

	if(NULL == entry){
		return -1;
	}

	CLIEntryList_t *tmpEntry = (CLIEntryList_t *)malloc(sizeof(CLIEntryList_t));
	if(NULL == tmpEntry){
		return -1;
	}
	tmpEntry->entry = entry;
	tmpEntry->next = NULL;

	if(NULL == CLIListHead){
		CLIListHead = tmpEntry;
		return 0;
	}

	while(NULL != pEntry->next){
		pEntry = pEntry->next;
	}
	pEntry->next = tmpEntry;

	return 0;
}

void CLI_Init(void)
{
    __CLI_ADDENTRY(Echo);
    __CLI_ADDENTRY(ShowVersion);
    
    CLI_Prompt();
}

/* parse command line and fill structure */
static int CLI_ParseCommand(void)
{
	CLIEntryList_t	*pCLIEntry;
#if CLI_ARGS_NUM_MAX
	u8 		j = 0;
	char	*token = NULL;
	char	*pParam[CLI_ARGS_NUM_MAX] = {NULL};

	token = strtok(CmdBuf, CLI_DELIM_STR);
#endif

	for(pCLIEntry = CLIListHead; pCLIEntry != NULL; pCLIEntry = pCLIEntry->next)
        {
            if(!strcmp(CmdBuf, pCLIEntry->entry->cmd))
            {
#if CLI_ARGS_NUM_MAX
                    /* Parse param */
                    if(NULL != token)
                    {
                        for(j = 0; j < pCLIEntry->entry->param_num; j++)
                        {
                                token = strtok(NULL, CLI_DELIM_STR);
                                if(token != NULL)
                                {
                                        pParam[j] = token;
                                }
                                else
                                {
                                        break;
                                }
                        }
                    }
                    if(NULL != pCLIEntry->entry->cmd_handler)
                    {
                        return pCLIEntry->entry->cmd_handler(j, pParam);
                    }
#else
                    if(NULL != pCLIEntry->entry->cmd_handler)
                    {
                            return pCLIEntry->entry->cmd_handler(0, NULL);
                    }
#endif
            }/*end of if(!strcmp(CmdBuf, pCLIEntry->entry->cmd))*/
	}/*end of for(pCLIEntry = CLIListHead; pCLIEntry != NULL; pCLIEntry = pCLIEntry->next)*/
        printf("\rError Command!\n");
	return -1;
}

void CLI_RevCB(u8 Chr)
{
    static u8 BufPos = 0;

    if(EchoState)
        putchar(Chr);
    
//    if(Chr == '\n')  //回车键不处理
//    {
//        return;
//    }

    /*如果是命令结束符，解析命令*/
    if(CLI_CMD_END_CHAR == Chr)
    {
        if(0 != BufPos){
            CmdBuf[BufPos] = 0;
            CLI_ParseCommand(); //解析命令并执行
	}
        BufPos = 0;             //清空命令行
        CLI_Prompt();
        return;
    }
    
    if(BufPos < CLI_CMD_BUF_LEN)
    {                                   /*不是结束符且小于最长字字符串*/
      CmdBuf[BufPos++] = Chr;
    }
    else
    {                                   /*超过长度，清零*/
      BufPos = 0;
    }
}