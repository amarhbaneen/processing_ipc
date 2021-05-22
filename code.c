#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<sys/sem.h>
#include<sys/wait.h>
#include<errno.h>
#define SEGSIZE 100
#define SEMPERM 0600
typedef struct {
    int id; // thread id
    int index;
    int value;
    int oldvalue;
    int isPrimary;

} Theard;

typedef union _semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}semun;
int initsem(key_t semkey)
{
    int status = 0, semid;

    //int semget(key_t key, int nsems, int semflg);
    if( ( semid = semget(semkey, 1, SEMPERM | IPC_CREAT | IPC_EXCL ) ) == -1 )
    {
        if( errno == EEXIST )
        {
            semid = semget( semkey, 1, 0 );
        }
    }
    else
    {
        union _semun arg;

        arg.val =1;

        // int semctl(int semid, int semnum, int cmd, union semun arg);
        status = semctl(semid, 0, SETVAL, arg);
    }
    if( semid == -1 || status == -1 )
    {
        perror("Error initsem");
        exit(-1);
    }
    return (semid);
}
void process_update(Theard* Theard_arr , int index,int size)
{

    if(Theard_arr[index].isPrimary == 0)
    {
        int pre_value = Theard_arr[size-1].value;
        if (Theard_arr[index].value == pre_value)
        {
            Theard_arr[index].oldvalue = Theard_arr[index].value;
            Theard_arr[index].value = (pre_value+1)% size;
            printf("permition to write at process %d with value %d\n",index,Theard_arr[index].value);
        }
        else{
            Theard_arr[index].oldvalue = Theard_arr[index].value;
        }

    }
    else{
        int pre_value = Theard_arr[index-1].oldvalue;
        if(Theard_arr[index].value != pre_value){
            Theard_arr[index].oldvalue = Theard_arr[index].value;
            Theard_arr[index].value = pre_value;
            printf("permition to write at process %d with value %d \n",index,Theard_arr[index].value);
        }
        else{
            Theard_arr[index].oldvalue = Theard_arr[index].value;

        }

    }
}
void child_run(int sem_set_id,Theard* theard_arr,int size,int index)
{

    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0 ;
    semop(sem_set_id,&sem_op,1);
    process_update(theard_arr,index,size);
    struct sembuf v_buf;
    v_buf.sem_num = 0;
    v_buf.sem_op = 1;
    v_buf.sem_flg = 0;
    semop(sem_set_id, &v_buf, 1);




}
void init_arr(Theard* arr,int size)
{

    for(int i=0;i<size;i++)
    {



        if(i == 0)
        {
            arr[i].isPrimary = 0;
            arr[i].index = i;
            arr[i].value = i;
            arr[i].oldvalue =i;
        }
        else{
            arr[i].isPrimary = 1;
            arr[i].index = i;
            arr[i].value = i;
            arr[i].oldvalue =i;
        }
    }
}
void init_thread(Theard** arr,int size,int shmid)
{

    for(int i=0;i<size;i++)
    {

        arr[i] = shmat(shmid, 0, 0);
    }
}
void update_arr(Theard* arr,int size){
    for(int i=0;i<size;i++) {
        arr[i].oldvalue=arr[i].value;


    }


}
void pulse(int sem_set_id,Theard* arr,int size,int r_pid)
{
    for(int i=0;i<size;i++)
    {
        r_pid = fork();
        switch (r_pid)
        {
            case -1:
                printf("FORK FAILED \n");
                exit(0);
            case 0:

                child_run(sem_set_id,arr,size,i);
                exit(0);

            default:
                break;
        }
    }

    for(int i=0;i<size;i++){
        wait(NULL);
    }



}
void showvalue(Theard* arr,int size)
{
    for(int i=0;i<size;i++)
    {
        printf("The process %d has value %d \n",i,arr[i].value);
    }
}
int main()
{
    key_t key;
    key_t sem_key;
    int   shmid;

    int threads_quanity;
    Theard* theard_arr ;
    printf("enter the number of processes you want \n");
    scanf("%d",&threads_quanity);
    int r_pid;
    key = ftok(".", 777); // create key
    sem_key = ftok(".", 999); // create key
    int sem_set_id = initsem(sem_key);
    if((shmid = shmget(key, threads_quanity* sizeof(Theard), IPC_CREAT|IPC_EXCL|0666)) == -1) //create shared memory
    {
        printf("Shared memory segment exists\n");
    }
    if((theard_arr = (Theard *)shmat(shmid, 0, 0)) == NULL) // create pointer to shared memory
    {
        perror("shmat");
        exit(1);
    }

    init_arr(theard_arr,threads_quanity);
    printf("menu : \n 1. show values \n 2.Pulse \n 0.exit \n");
    int user_input;
    scanf("%d",&user_input);

    while(user_input != 0)
    {
        if(fork()==0) {

            switch (user_input) {
                case 1:
                    showvalue(theard_arr, threads_quanity);
                    exit(0);
                case 2:
                    pulse(sem_set_id, theard_arr, threads_quanity, r_pid);
                    update_arr(theard_arr, threads_quanity);

                    exit(0);
                case 0:
                    exit(0);

                default:
                    break;
            }
        }
        else{
            wait(NULL);
            printf("menu : \n 1. show values \n 2.Pulse \n 0.exit \n");
            scanf("%d",&user_input);
        }


    }
    shmctl(shmid, IPC_RMID, 0);
    return 0;
}