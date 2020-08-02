/********************************************************************
** Copyright (c) 2018-2020 Guan Wenliang
** This file is part of the Berry default interpreter.
** skiars@qq.com, https://github.com/Skiars/berry
** See Copyright Notice in the LICENSE file or at
** https://github.com/Skiars/berry/blob/master/LICENSE
********************************************************************/
#include "berry.h"
#include "be_repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

/* using GNU/readline library */
#if defined(USE_READLINE_LIB)
    #include <readline/readline.h>
    #include <readline/history.h>
#endif

/* portable readline function package */
static char* get_line(const char *prompt)
{
#if defined(USE_READLINE_LIB)
    char *line = readline(prompt);
    if (line && strlen(line)) {
        add_history(line);
    }
    return line;
#else
    static char buffer[1000];
    fputs(prompt, stdout);
    fflush(stdout);
    if (be_readstring(buffer, sizeof(buffer))) {
        buffer[strlen(buffer) - 1] = '\0';
        return buffer;
    }
    return NULL;
#endif
}

static void free_line(char *ptr)
{
#if defined(USE_READLINE_LIB)
    free(ptr);
#else
    (void)ptr;
#endif
}

void sig_handler(int signo)
{
  if (signo == SIGUSR1) {
    printf("[VM]: Callback time!\n");
    signal(SIGUSR1, sig_handler);
  }
}

static int cbftest(bvm *vm)
{
    int top = be_top(vm); // Get the number of arguments
    /* Verify the number and type of arguments */
    if (top == 1 && be_isfunction(vm, 1)) {
        printf("[VM]: Register time! %s\n", be_typename(vm, 1));
    }
    be_return_nil(vm); // Return nil when something goes wrong
}


int main()
{
    if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
        printf("\ncan't catch SIGUSR1\n");
    }
    bvm *vm = be_vm_new(); /* create a virtual machine instance */
    be_regfunc(vm, "cbftest", cbftest); // Register the native function "cbftest"
    int res = be_loadmode(vm, "cbftest/cbftest.be", bfalse);
    if (res == BE_OK) { /* parsing succeeded */
        res = be_pcall(vm, 0); /* execute */
    }
    res = be_repl(vm, get_line, free_line);
        if (res == -BE_MALLOC_FAIL) {
            be_writestring("error: memory allocation failed.\n");
        }
    be_vm_delete(vm); /* free all objects and vm */
    return res;
}

