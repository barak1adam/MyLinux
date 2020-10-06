#include <unistd.h>       //Support all standards    
#include <sys/mman.h>     //Memory locking functions 

#define DATA_SIZE 2048



lock_memory(char  addr,size_t  size)
{
  unsigned long    page_offset, page_size;

  page_size = sysconf(_SC_PAGE_SIZE);
  page_offset = (unsigned long) addr % page_size;

  addr -= page_offset;   //Adjust addr to page boundary 
  size += page_offset;   //Adjust size with page_offset 

  return ( mlock((void*)addr, size) );   //Lock the memory 
}

unlock_memory(char   addr, size_t  size)
{
  unsigned long    page_offset, page_size;

  page_size = sysconf(_SC_PAGE_SIZE);					// the same as getpagesize()
  page_offset = (unsigned long) addr % page_size;

  addr -= page_offset;   //Adjust addr to page boundary 
  size += page_offset;   //Adjust size with page_offset 

  return ( munlock(addr, size) );   //Unlock the memory 
}

main()
{
  char data[DATA_SIZE];

  if ( lock_memory(data, DATA_SIZE) == -1 )
    perror(lock_memory);

//            Do work here 

  if ( unlock_memory(data, DATA_SIZE) == -1 )
    perror(unlock_memory);
}

