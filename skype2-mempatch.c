/*
   Copyright (C) 2015-2017, Rudolf Sikorski <rudolf.sikorski@freenet.de>

   This file is part of the `skype2-mempatch' program.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
    USAGE:
    1. Run Skype 2.2.0.35 or another unsupported version
    2. Wait login window
    3. Enter username and password, check 'Sign me in when Skype starts'
    4. Run skype2-mempatch $(pidof skype) '2.2.0.35' '8.10.0.9'
       where '2.2.0.35' - our Skype version, '8.10.0.9' - latest version for Windows
    5. Click 'Sign in'
    6. After Skype signing in, close skype or wait of crash :)
    7. Restart Skype
*/

#define PROGRAM_NAME    (argv[0])
#define TARGET_PID      (argv[1])
#define REPLACE_BEFORE  (argv[2])
#define REPLACE_AFTER   (argv[3])

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char * argv[])
{
    int pipe_to[2], pipe_from[2];
    pid_t cpid;
    if(argc != 4)
    {
        fprintf(stderr, "Usage: %s <pid> <old_version_string> <new_version_string>\n", PROGRAM_NAME);
        exit(EXIT_FAILURE);
    }
    if(pipe(pipe_to) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if(pipe(pipe_from) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "# skype2-mempatch version 0.3 alpha\n");
    fprintf(stderr, "# Copyright (C) 2015-2017, Rudolf Sikorski <rudolf.sikorski@freenet.de>\n");
    fprintf(stderr, "\n");
    fflush(stderr);

    if((cpid = fork()) == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if(cpid == 0) /* Child process */
    {
        char * scanmem_argv[2];
        scanmem_argv[0] = PROGRAM_NAME;
        scanmem_argv[1] = TARGET_PID;

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        close(pipe_to[1]);
        close(pipe_from[0]);

        if(dup2(pipe_to[0], STDIN_FILENO) == -1)
            exit(EXIT_FAILURE);
        if(dup2(pipe_from[1], STDOUT_FILENO) == -1)
            exit(EXIT_FAILURE);
        if(dup2(pipe_from[1], STDERR_FILENO) == -1)
            exit(EXIT_FAILURE);

        /* Run scanmem */
        int scanmem_main(int, char **);
        if(scanmem_main(sizeof(scanmem_argv) / sizeof(char *), scanmem_argv))
            exit(EXIT_FAILURE);

        close(pipe_to[0]);
        close(pipe_from[1]);
        exit(EXIT_SUCCESS);
    }
    else
    {
        char chr, buf[255];
        int addr_num, i, j, flag;
        char ** addr;
        FILE * pipefile = fdopen(pipe_to[1], "w");
        close(pipe_to[0]);
        close(pipe_from[1]);

        /* Read head scanmem message */
        while(read(pipe_from[0], & chr, 1) > 0)
        {
            write(STDERR_FILENO, & chr, 1);
            if(chr == '>') break;
        }

        /* We will search strings */
        fprintf(pipefile, "option scan_data_type string\n");
        fflush(pipefile);

        /* Read response */
        while(read(pipe_from[0], & chr, 1) > 0)
        {
            write(STDERR_FILENO, & chr, 1);
            if(chr == '>') break;
        }

        /* Hmm... Why not? */
        fprintf(pipefile, "option region_scan_level 3\n");
        fflush(pipefile);

        /* Read response */
        while(read(pipe_from[0], & chr, 1) > 0)
        {
            write(STDERR_FILENO, & chr, 1);
            if(chr == '>') break;
        }

        /* We will search version string */
        fprintf(pipefile, "\" %s\n", REPLACE_BEFORE);
        fflush(pipefile);

        /* Read response */
        i = 0;
        while(read(pipe_from[0], & chr, 1) > 0)
        {
            write(STDERR_FILENO, & chr, 1);
            if(chr == '\n') i = 0;
            else if(chr == '>') break;
            else buf[i++] = chr;
        }
        buf[i] = '\0';

        /* Allocate some memory */
        addr_num = atoi(buf);
        fprintf(stderr, "# Match size: %d\n", addr_num);
        addr = (char **)calloc(sizeof(char *), addr_num);
        for(i = 0; i < addr_num; i++)
            addr[i] = (char *)calloc(sizeof(char), 255);

        /* Get list of matches */
        fprintf(pipefile, "list\n");
        fflush(pipefile);

        /* Read response */
        i = j = flag = 0;
        while(read(pipe_from[0], & chr, 1) > 0)
        {
            write(STDERR_FILENO, & chr, 1);
            if(chr == '\n' && flag == 2)
            {
                i = 0;
                j++;
                flag = 0;
            }
            else if(chr == '>')
                break;
            else if(chr == ']' && flag == 0)
                flag = 1;
            else if(chr == ',')
            {
                flag = 2;
                addr[j][i++] = '\0';
            }
            else if(flag == 1)
                addr[j][i++] = chr;
        }

        /* And replace strings :) */
        for(i = 0; i < addr_num; i++)
        {
            char * t = strrchr(addr[i], ' ');
            if(!t) t = addr[i];
            else t++;
            fprintf(pipefile, "write string %s %s\n", t, REPLACE_AFTER);
            fflush(pipefile);

            while(read(pipe_from[0], & chr, 1) > 0)
            {
                write(STDERR_FILENO, & chr, 1);
                if(chr == '>') break;
            }
        }

        /* Exit from scanmem */
        fprintf(pipefile, "q\n");
        fflush(pipefile);
        while(read(pipe_from[0], & chr, 1) > 0)
            write(STDERR_FILENO, & chr, 1);

        fclose(pipefile);
        close(pipe_from[0]);
        close(pipe_to[1]);
        for(i = 0; i < addr_num; i++)
            free(addr[i]);
        free(addr);

        fprintf(stderr, "\n# Complete!\n");

        wait(NULL);
    }

    return EXIT_SUCCESS;
}

