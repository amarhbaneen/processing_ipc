 program  built by Amar alsana ID :208971549 , Tmem nsasra 208283119

###############################################################
u must have those libarires and include them in the code      #
#include <stdio.h>                                            #
#include <string.h>                                           #
#include <stdlib.h>                                           #
#include <sys/types.h>                                        #
#include <sys/ipc.h>                                          #
#include <sys/shm.h>                                          #
#include<sys/sem.h>                                           #
#include<sys/wait.h>                                          #
#include<errno.h>                                             #
to use the program you need to install the following Packages #
 gcc                                                          #
"sudo apt-get install gcc"                                    #
###############################################################

#############################  Instructions to run the program ##############################################
the program is about usage of processing with semaphore using IPC                                           #
 our example is to connect every process with another one and sending the value of the process to it        #
 we used an algorith that based on using struct of process ( we named it therad)                            #
 the struct have 5 elemnts                                                                                  #
 - id; // process id                                                                                        #
 - index //index of the process                                                                             #
 - value //  the current value of the process                                                               #
 - oldvalue // old value of the process cause we have to send it to the next process                        #
 - isPrimary // if the process is primary there  other condtion for sending value # 0 is TRUE 1 FALSE       #
we got stock with the value cause if the value of current process and next are equal it doesnt change       #
so we changed the old value of current process to the current value so the next process cant be equal       #
                                                                                                            #
to run the program you need to run the command                                                              #
" gcc code.c"                                                                                               #
after this command you should run                                                                           #
"./a.out "                                                                                                  #
 it will ask you for entering number of process                                                             #
 after that will open menu                                                                                  #
 1. show values // it will show the value of all process and its id                                         #
 2.Pulse // will start pulsing the value for another process (next)                                         #
 0.exit  // it will exit the  program                                                                       #
 if you got stuck with " Shared memory segment exists "  it cause you already written to this key           #
 you should the key change (line 191 and line 192) to another key who isnt busy at RAM                      #
#############################################################################################################