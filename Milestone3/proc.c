#include "proc.h"

/*
 * This function initializes all of the global variables and 
 * structures defined above.
 * All entries in the memory map should be marked as FREE.
 * 
 * All of the PCBs in the pcbPool should have the first character of their 
 * names set to null, their state set to DEFUNCT and their segment and stack
 * pointers set to 0x00. 
 * 
 * The PCB for the idle process should be initialized with the name IDLE, 
 * the state READY, and both the segment and stack pointers set to 0x00.
 *
 * The PCB for the running process should refer to the PCB for the idle process.
 *
 * The ready queue should be empty.
 */ 
void initializeProcStructures() {
    int i;
  
    for(i = 0; i < MAX_SEGMENTS; ++i) {
        pcbPool[i].index = EMPTY_INDEX;
        pcbPool[i].state = DEFUNCT;
        pcbPool[i].segment = 0x00;
        pcbPool[i].stackPointer = 0x00;
        pcbPool[i].parentSegment = NO_PARENT;
    }

    /*
     * The PCB for the idle process should be initialized with the name IDLE, 
     * the state READY, and both the segment and stack pointers set to 0x00.
     */
    idleProc.index = EMPTY_INDEX;
    idleProc.state = READY;
    idleProc.segment = 0x00;
    idleProc.stackPointer = 0x00;
    idleProc.parentSegment = NO_PARENT;
    
    /*
     * The PCB for the running process should refer to the PCB for the idle process     
    */  
    running = &idleProc;
     
    // the ready queue should be empty.
    readyHead = NULL;
    readyTail = NULL;
}

/*
 * Returns the first free memory segment or -1 if 
 * all of the memory segments are used.  The returned memory segment
 * should be marked as used.
 */
int getFreeMemorySegment() {
	int i;
	for (i = 0; i < MAX_SEGMENTS; ++i) {
		if (memoryMap[i] == SEGMENT_FREE) {
			memoryMap[i] = SEGMENT_USED;
			return (i + 2) << 12;
		}
	}
	return NO_FREE_SEGMENTS;
}

/*
 * Release the memory segment indicated by the segment number into
 * the memory map.
 */
void releaseMemorySegment(int segment) {
  unsigned int seg = segment;
	memoryMap[(seg >> 12) - 2] = SEGMENT_FREE;
}

/*
 * Return a pointer to an available PCB from the PCB pool.  All of the
 * available PCBs should be marked as DEFUNCT.  The returned PCB should
 * be marked as STARTING. Return NULL if there are no PCBs available.
 */
struct PCB *getFreePCB() {

	int i;
  
	for(i = 0; i < MAX_SEGMENTS; ++i) {
		if (pcbPool[i].state == DEFUNCT) {
			pcbPool[i].state = STARTING;
      return pcbPool + i;
    }
  }
  
	return NULL;
}

/*
 * Release the provided PCB by setting its state to DEFUNCT, setting
 * its next and prev pointers to NULL and setting the first character
 * of its name to 0x00.
 */
void releasePCB(struct PCB *pcb) {
	pcb->state = DEFUNCT;
	pcb->next = NULL;
	pcb->prev = NULL;
	pcb->index = EMPTY_INDEX;
	pcb->segment = 0x00;
	pcb->stackPointer = 0x00;
	pcb->parentSegment = NO_PARENT;
}

/*
 * Add the provided PCB to the tail of the ready queue.
 */
void addToReady(struct PCB *pcb) {
	if (readyHead == NULL) {	// Empty queue
		readyHead = pcb;
		readyTail = pcb;
		pcb->next = NULL;
		pcb->prev = NULL;
	}
	else {
		readyTail->next = pcb;
		pcb->prev = readyTail;
		pcb->next = NULL;
		readyTail = pcb;
	}
}

/*
 * Remove the PCB at the head of the ready queue and return a 
 * pointer to it.
 */
 
struct PCB *removeFromReady() {
	
	if (readyHead == NULL) {		// empty queue
		return NULL;
	}
	else if (readyHead == readyTail) {		// one entry in queue
		struct PCB *tmp = readyHead;
		readyHead = NULL;
		readyTail = NULL;
		return tmp;
	}
	else {
		struct PCB *tmp = readyHead;
		readyHead = readyHead->next;
		readyHead->prev = NULL;
		return tmp;
	}
}

struct PCB *getPCBOfSegment(int segment) {	
	int i;
	for (i = 0; i < MAX_SEGMENTS; ++i) {
		if (pcbPool[i].segment == segment) {
			return pcbPool + i;
    }
	}
	return NULL;	
}
