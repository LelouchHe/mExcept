#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define MAX_BUF 1024
#define MAX_STACKINFO 32
#define PIPE_NUM 2

#define QUIT "quit"
#define ACK "ack"

#define ADDR2LINE_PRE "addr2line -f -s -e "
#define EXCEPT_PRE "except"

static void process_cmd(int p2c[], int c2p[])
{
    char buf[MAX_BUF];

    close(p2c[1]);
    close(c2p[0]);

    dup2(p2c[0], STDIN_FILENO);
    close(p2c[0]);

    dup2(c2p[1], STDOUT_FILENO);
    close(c2p[1]);

    while (read(STDIN_FILENO, buf, MAX_BUF))
    {
        if (strncmp(buf, QUIT, strlen(QUIT)) == 0)
            write(STDOUT_FILENO, ACK, strlen(ACK));
        else
            system(buf);
    }
}

static char *get_cmd_pre(char cmd[], int len)
{
    int cmd_len;

    sprintf(cmd, ADDR2LINE_PRE);
    cmd_len = strlen(cmd);
    cmd_len += readlink("/proc/self/exe", cmd + cmd_len, len - cmd_len - 1);
    cmd[cmd_len] = '\0';
    return cmd;
}

// TODO:
static int filter(char result[], int len)
{
    return 0;
}

static int print_result(char result[], int len, int index)
{
    char *s, *p;

    if (filter(result, len) == 1)
        return 1;

    s = result;
    
    fprintf(stderr, "(%d)", index);

    p = strchr(s, '\n');
    *p = '\0';
    fprintf(stderr, "%-20s", s);
    
    s = p + 1;
    p = strchr(s, ':');
    *p = '\0';
    fprintf(stderr, "%10s:", s);

    s = p + 1;
    p = strchr(s, '\n');
    *p = '\0';
    fprintf(stderr, "%-5s\n", s);

    if (strncmp(result, "main", 4) == 0)
        return 0;
    else
        return 1;
}

void util_dumpstack()
{
    int p2c[PIPE_NUM];
    int c2p[PIPE_NUM];
    int pid;

    pipe(p2c);
    pipe(c2p);

    pid = fork();
    if (pid == 0)
    {
        process_cmd(p2c, c2p);
    }
    else
    {
        char cmd[MAX_BUF];
        char buf[MAX_BUF];

        void *stackinfo[MAX_STACKINFO];
        int stack_num;
        int i;

        close(p2c[0]);
        close(c2p[1]);
        
        get_cmd_pre(cmd, sizeof (cmd));

        stack_num = backtrace(stackinfo, MAX_STACKINFO);

        for (i = 0; i < stack_num; i++)
        {
            sprintf(buf, "%s %p\n", cmd, stackinfo[i]);
            write(p2c[1], buf, strlen(buf));
            read(c2p[0], buf, MAX_BUF);
            
            if (print_result(buf, strlen(buf), i) == 0)
                break;
        }

        sprintf(buf, QUIT);
        write(p2c[1], buf, strlen(buf));

        read(c2p[0], buf, MAX_BUF);
        assert(strncmp(buf, ACK, strlen(ACK)) == 0);
        
        close(p2c[1]);
        close(c2p[0]);
    }
}
