/*
** SCCS ID:	@(#)ulib.h	1.1	4/17/15
**
** File:	ulib.h
**
** Author:	CSCI-452 class of 20145
**
** Contributor:
**
** Description:	User-level library declarations
**
** These are separate from the kernel library so that user processes
** aren't trying to use code that is in the kernel's address space,
** in case anyone wants to implement real memory protection.
*/

#ifndef _ULIB_H_
#define _ULIB_H_

#include "types.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef __SP_ASM__

#include "process.h"

/*
** Start of C-only definitions
*/

/*
** Types
*/

/*
** Globals
*/

/*
** Prototypes
*/

/*
** itos10() - convert an int into a decimal number
*/

int itos10( char *buf, int value );

/*
** itos8() - convert an int into an octal number (1 to 11 digits)
*/

int itos8( char *buf, int value, int store_all );

/*
** itos16() - convert and into into a hex number (1 to 8 digits)
*/

int itos16( char *buf, int value, int store_all );

/*
** default exit routine for processes
*/

void __default_exit__( void );

/*
** exit - terminate the calling process
**
** usage:	exit();
**
** does not return
*/

void exit( void );

/*
** spawn - create a new process running a different program
**
** usage:	pid = spawn(entry);
**
** returns:
**	pid of the spawned process, or -1 on failure
*/

int32_t spawn( void (*entry)(void) );

/*
** spawnp - create a new process running a different program
**
** usage:	pid = spawnp(entry,prio);
**
** returns:
**	pid of the spawned process, or -1 on failure
*/

int32_t spawnp( void (*entry)(void), uint8_t prio );

/*
** sleep - put the current process to sleep for some length of time
**
** usage:	sleep(ms);
**
** if the sleep time (in milliseconds) is 0, just preempts the process;
** otherwise, puts it onto the sleep queue for the specified length of
** time
*/

void sleep( uint32_t ms );

/*
** read - read from the console or SIO
**
** usage:	n = read( fd, buf, size );
**
** reads up to 'size' characters from 'fd', placing them in 'buf'
**
** returns:
**      the number of characters placed into 'buf', or -1 on error
*/

int read( int fd, char *buf, int size );

/*
** write - write to the console or SIO
**
** usage:	n = write( fd, buf, size );
**
** writes 'size' characters from 'buf' to 'fd'
**
** returns:
**      the number of characters written
*/

int write( int fd, char *buf, int size );

/*
** get_process_info - retrieve information about a process
**
** usage:	n = get_process_info( what, who )
**
** returns:
**      the desired value, or -1 on error
**
** if 'who' is 0, retrieves information about the calling process
*/

int32_t get_process_info( uint32_t what, uint16_t who );

/*
** get_system_info - retrieve information about the system
**
** usage:	n = get_system_info( what )
**
** returns:
**      the desired value, or -1 on error
*/

int32_t get_system_info( uint32_t what );


/*
** create_file - create a file in the file system
**
** usage:	n = create_file( char* filename )
**
** returns:
**      0 for success, 1 for failure
*/
uint8_t create_file( char* filename );

/*
** delete_file - delete a file in the file system
**
** usage:	n = delete_file( char* filename )
**
** returns:
**      0 for success, 1 for failure
*/
uint8_t delete_file( char* filename );

/*
** write_file - write to a file in the file system
**
** usage:	n = write_file( char* filename, uint8_t size, char* buf, 
**			int doAppend )
**
** returns:
**      0 for success, 1 for failure
*/
uint8_t write_file( char* filename, uint16_t size, char* buffer, int doAppend );

/*
** read_file - reads from a file in the file system
**
** usage:	n = read_file( char* filename )
**
** returns:
**      0 for failure, or pointer to data
*/
uint8_t read_file( char* filename );

/*
** list_files - list the files in the file system
**
** usage:	n = list_files( void )
**
** returns:
**      pointer for string result
*/
uint8_t list_files( void );

/*
** bogus - a bogus system call, for testing our syscall ISR
**
** usage:	bogus();
*/

void bogus( void );

#endif

#endif
