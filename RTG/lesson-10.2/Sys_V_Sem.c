/*
 * Sys_V_Sem - a program to demonstrate usage of System V Semaphores.
 *

Description
-----------
This program creates and initializes a semaphore. 
 
 
 Missions
-----------
 1.Once you run the program use the ipcs command from the command line to verify that the semaphore exists
 2 Lock the Shared Memory program using the semaphore.
 3. What is the key used for ? 
 4. Try to call the program from different terminals.
 5. Destroy the semaphore (ipcrm - could be used to accomplish this)
 
 To compile me for Linux, use: gcc Sys_V_Semaphore.c -ggdb -o sem 


To execute, type:  ./sem
 */




/*************  includes     *****************/
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/sem.h> 

#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 



/*************  definitions     *****************/
union semun 
{
	int val;
    struct semid_ds *buf;
    ushort *array;
};

/*************  Prototypes   *****************/
int init_sys_v_sem();
int lock_sys_v_sem();
int unlock_sys_v_sem();


/*************  Globals   *****************/



/*************  main() function ****************/
int main()
{ 
	
    int ret=-1;
    ret = init_sys_v_sem();
    
    printf("Press return to lock: "); 
    getchar(); 
    printf("Trying to lock...\n");  
    ret = lock_sys_v_sem();
    printf("Locked.\n"); 
        
    /*
     *  	do your thing atomicly
     */
        
    printf("Press return to unlock: ");     
    getchar(); 
    ret = unlock_sys_v_sem();
    printf("Unlocked\n"); 
        
    return 0; 
}

/*************  int init_sys_v_sem() function ****************/
int init_sys_v_sem()
{
	key_t key; 
    int semid; 
    union semun arg; 
     
    if ((key = ftok("Sys_V_Sem.c", 'J')) == -1) 
    { 
		perror("ftok"); 
        exit(1); 
    }
    
    /* create a semaphore set with 1 semaphore: */ 
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) 
    { 
        perror("semget"); 
        exit(1); 
    }
     
    /* initialize semaphore #0 to 1: */ 
    arg.val = 1; 
    if (semctl(semid, 0, SETVAL, arg) == -1) 
    { 
        perror("semctl"); 
        exit(1); 
    } 
	return 0;
}
/*************  int lock_sys_v_sem() function ****************/
int lock_sys_v_sem()
{
	key_t key; 
    int semid; 
    struct sembuf sb = {0 /*sem index*/, -1 /*dec or wait*/, 0 /*no flag*/};  /* set to allocate resource */ 
    if ((key = ftok("Sys_V_Sem.c", 'J')) == -1) 
    { 
		perror("ftok"); 
        exit(1); 
    }
     
    /* grab the semaphore set created by seminit.c: */ 
    if ((semid = semget(key, 1, 0)) == -1) { 
            perror("semget"); 
            exit(1); 
    }
    
    //lock resource
    if (semop(semid, &sb, 1) == -1) 
    { 
		perror("semop"); 
        exit(1); 
    } 

	return 0;
}
/*************  int unlock_sys_v_sem() function ****************/
int unlock_sys_v_sem()
{
	key_t key; 
    int semid; 
    struct sembuf sb = {0, 1, 0};  /* set to allocate resource */ 
    if ((key = ftok("Sys_V_Sem.c", 'J')) == -1) 
    { 
		perror("ftok"); 
        exit(1); 
    }
     
    /* grab the semaphore set created by seminit.c: */ 
    if ((semid = semget(key, 1, 0)) == -1) { 
            perror("semget"); 
            exit(1); 
    }
    
    //increment or post
    sb.sem_op = 1; 
    
    /* free resource */ 
    if (semop(semid, &sb, 1) == -1) 
    { 
		perror("semop"); 
        exit(1); 
    } 

	return 0;
}

