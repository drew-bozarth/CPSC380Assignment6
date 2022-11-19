#include <string.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

const int tlbSize = 16;
const int pageTableSize = 256;
const int physicalMemorySize = 256;
const int bufferSize = 256;


struct TLB {
	unsigned char page[16];
	unsigned char frame[16];
	int index;
};

int readDisk(int pageNumber, int* openFrame, char *physicalMemory) {
	char buffer[bufferSize];
	memset(buffer, 0, sizeof(buffer));

	FILE *fileOpen;
	fileOpen = fopen("BACKING_STORE.bin", "rb");
	if (fileOpen == NULL) {
		printf("File could not open, try again");
	}
	if (fseek(fileOpen, pageNumber * physicalMemorySize, SEEK_SET) !=0)
		printf("Error occured in fseek");
	if (fread(buffer, sizeof(char), physicalMemorySize, fileOpen) == 0)
		printf("Error occured in fread");
	

	for(int i = 0; i < physicalMemorySize; i++){
		*((physicalMemorySize * (*openFrame) + physicalMemory) + i) = buffer[i];
	}
	
	(*openFrame)++;
	return (*openFrame) - 1;
}

int findPage(int logicalAddress, struct TLB *tlb, char* pageTable, int* openFrame, char* physicalMemory, int* numHits, int* pageFaults){

	unsigned char pageNumber;
	unsigned char offset;
	bool tlbHit = false;
	int newFrame = 0;
	int oldFrame = 0;
	int val;
	unsigned char mask = 0xFF;

	FILE *outputFile = fopen("sampleOutput.txt", "a");
	
	if (outputFile == NULL)
	{
		printf("Could not open sample output file!\n");
		exit(1);
	}

	printf("Logical adress: %d\t", logicalAddress);
	fprintf(outputFile, "Logical adress: %d\t", logicalAddress);

	pageNumber = (logicalAddress >> 8) & mask;
	offset = logicalAddress & mask;
	
	for (int i = 0; i < tlbSize; i++) {
		if(tlb->page[i] == pageNumber){
			oldFrame = tlb->frame[i];
			(*numHits)++;
			tlbHit = true;
		}
	}

	if (tlbHit == false){
		if (pageTable[pageNumber] != -1){

		}
		else {
			newFrame = readDisk(pageNumber, openFrame, physicalMemory);
			(*pageFaults)++;
			pageTable[pageNumber] = newFrame;
		}
		oldFrame = pageTable[pageNumber];
		tlb->page[tlb->index] = pageNumber;
		tlb->frame[tlb->index] = pageTable[pageNumber];
		tlb->index = (tlb->index + 1) % tlbSize;
	}
	int index = ((unsigned char) oldFrame * physicalMemorySize) + offset;
	val = *(physicalMemory + index);
	printf("Physical address: %d\t Signed Byte Value: %d\n", index, val);
	fprintf(outputFile, "Physical address: %d\t Signed Byte Value: %d\n", index, val);

	fclose(outputFile);

	return 0;
}

int main (int argc, char* argv[]){
	int val;
	FILE *fileOpen;
	struct TLB tlb;

	


	int numHits = 0;
	int inputCount = 0;
	int pageFaults = 0;
	int openFrame = 0;
	
	float hitRate;
	float pfRate;

	char physicalMemory[physicalMemorySize][physicalMemorySize]; 
	char pageTable[pageTableSize];

	memset(pageTable, -1, sizeof(pageTable));	
	memset(tlb.page, -1, sizeof(tlb.page));
	memset(tlb.frame, -1, sizeof(tlb.frame));
	tlb.index = 0;

	if (argc < 2){
		printf("Not enough arguments, please enter more");
	}

	fileOpen = fopen(argv[1], "r");
	if (fileOpen == NULL){
		printf("File unable to open");
	}

	while (fscanf(fileOpen, "%d", &val) == 1){
		findPage(val, &tlb, pageTable, &openFrame, (char*)physicalMemory, &numHits, &pageFaults);
		inputCount++;
	}

	pfRate = (float)pageFaults / (float)inputCount;
	hitRate = (float)numHits / (float)inputCount;
	printf("Page Fault Rate = %.4f\nTLB hit rate= %.4f\n",pfRate, hitRate);

	FILE *outputFile = fopen("sampleOutput.txt", "a");
	
	if (outputFile == NULL)
	{
		printf("Could not open sample output file!\n");
		exit(1);
	}

	fprintf(outputFile, "Page Fault Rate = %.4f\nTLB hit rate= %.4f\n",pfRate, hitRate);

	fclose(outputFile);


	
	return 0;
}
	


