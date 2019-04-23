#ifndef GDROM_H
#define GDROM_H

//
// Initialize gdrom system
// returns the starting LBA of the main data track
//
int gdrom_init(void);

//
// Blocks until the prior read is done (you can ensure it won't block if you
// check gdrom_poll yourself), and then queues up a new read
//
void gdrom_readsectors(void *dest, int lba, int num);

//
// Returns nonzero if busy
//
int gdrom_poll(void);

#endif
