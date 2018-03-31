#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/time.h>

struct timeval tm1, tm2;

int cmpfuncs (const void * a, const void * b) {
		int * p = (int *)a;
		int * q = (int *)b;
		if( *p > *q)	return 1;
		else if( *p == *q)	return 1;
		else return 0;
}

//Basic

int generateRunFiles(char *inputFileName){
	FILE *inputFile;
	inputFile = fopen(inputFileName, "r+b");
	fseek(inputFile, 0L, SEEK_END);
	int sizeKey = ftell(inputFile)/sizeof(int);
	fseek(inputFile, 0L, SEEK_SET);
	int size = 000;
	while(sizeKey >= 1000){
		int inputBuffer[1000];
		fread(&inputBuffer, sizeof(int), 1000, inputFile);
		sizeKey -= 1000;
		qsort(&inputBuffer, 1000, sizeof(int), cmpfuncs);
		char name[20] = "input.bin.";
		char xx[20];
		sprintf(xx, "%03d", size);
		strcat(name, xx);
		FILE *temp = fopen(name, "w+b");
		fwrite(inputBuffer , sizeof(int) , 1000 , temp );
		size++;
		fclose(temp);
	}
	if(sizeKey != 0){
		int inputBuffer[sizeKey];
		fread(&inputBuffer, sizeof(int), sizeKey, inputFile);
		qsort(&inputBuffer, sizeKey, sizeof(int), cmpfuncs);
		char name[20] = "input.bin.";
		char xx[20];
		sprintf(xx, "%03d", size);
		strcat(name, xx);
		FILE *temp = fopen(name, "w+b");
		fwrite(inputBuffer , sizeof(int) , sizeKey , temp );
		fclose(temp);
	}		
	return 0;	
}

int basicSortRunFiles(char *inputFileName, char *outputFileName){
	int i, size = 000, val = 0, totalOutputBufferCount = 0, global_count = 0;
	int outputBuffer[1000];
	FILE *inputFile, *outputFile;
	inputFile = fopen(inputFileName, "r+b");
	outputFile = fopen(outputFileName, "w+b");
	fclose(outputFile);
	outputFile = fopen(outputFileName, "a+b");
	fseek(inputFile, 0L, SEEK_END);
	int sizeKey = ftell(inputFile)/sizeof(int);
	fseek(inputFile, 0L, SEEK_SET);
	int times = sizeKey/1000;
	if(sizeKey%1000 > 0) times++;
	int inputBuffer[1000];
	int each = 1000/times;
	int index[times], index_over[times], index_till[times], index_tell[times];
	for( i = 0; i < times; i++){
		index[i] = val;
		index_over[i] = each;
		index_till[i] = val + each;
		char name[20] = "input.bin.";
		char xx[20];
		sprintf(xx, "%03d", size);
		strcat(name, xx);
		FILE *temp = fopen(name, "r+b");
		fseek(temp, 0L, SEEK_SET);
		int store[each];
		fread(&store, sizeof(int), each, temp);
		index_tell[i] = ftell(temp);
		int j, k = 0;
		for( j = val; j < val + each; j++){
			inputBuffer[j] = store[k];
			k++;
		}
		size++;
		val += each;
		fclose(temp);
	}
	while(1 > 0){
		
		int min = 2147483647;
		int index_val = 0;
		for( i = 0; i < times; i++){
			if(((index_over[i] != -1)||(index[i] < index_till[i]))  && (inputBuffer[index[i]] < min)){
				min = inputBuffer[index[i]];
				index_val = i;
			}
		}
		if(min != 2147483647){
			global_count++;
			outputBuffer[totalOutputBufferCount] = min;
			totalOutputBufferCount++;
			index[index_val] += 1;
		}
		
		if((index[index_val] == index_till[index_val]) && index_over[index_val] != -1){
			int size = (index[index_val] - 1) / each;
			char name[20] = "input.bin.";
			char xx[20];
			sprintf(xx, "%03d", size);
			strcat(name, xx);
			FILE *temp = fopen(name, "r+b");
			fseek(temp, index_tell[index_val], SEEK_SET);
			int count = 0, buff;
			while(fread(&buff, sizeof(int), 1, temp) && count < each){
				inputBuffer[(index[index_val]-each) + count] = buff;
				count++;
			}
			index[index_val] -= each;
			index_till[index_val] -= each;
			index_till[index_val] += count;
			index_over[index_val] += count;
			if(count < each || index_over[index_val] == 1000)	index_over[index_val] = -1;
			else{
				index_tell[index_val] = ftell(temp) - sizeof(int);
			}
			fclose(temp);
		}
		if(totalOutputBufferCount == 1000){
			fwrite(outputBuffer, sizeof(int), totalOutputBufferCount, outputFile );
			totalOutputBufferCount = 0;
		}
		int cou = 0;
		for( i = 0; i < times; i++){
			if((index_over[i] == -1)&&(index[i] == index_till[i])) cou++;
		}
		if(cou == times)	break;
	}
	if(totalOutputBufferCount > 0){
		fwrite(outputBuffer, sizeof(int), totalOutputBufferCount, outputFile );
		totalOutputBufferCount = 0;
	}	
	fclose(outputFile);
}

// Multistep Merge

int generateRunFiles15(char *inputFileName){
	FILE *inputFile, *outputFile;
	int inputBuffer[1000], outputBuffer[1000], i, global_count = 0, k = 0, che = 0;
	inputFile = fopen(inputFileName, "r+b");
	fseek(inputFile, 0L, SEEK_END);
	int sizeKey = ftell(inputFile)/sizeof(int);
	fseek(inputFile, 0L, SEEK_SET);
	int times = sizeKey/1000;
	if(sizeKey%1000 > 0) times++;
	int super_runs = times / 15;
	int super_remaining_runs = times % 15;
	if(super_remaining_runs > 0)	che = 1;
	int size = 000, while_count = 0, each = 1000/15;
	for( k = 0; k < super_runs + che; k++){
		char name[20] = "input.bin.super.";
		char xx[20];
		sprintf(xx, "%03d", k);
		strcat(name, xx);
		FILE *temp = fopen(name, "w+b");
		fclose(temp);
	}
	while(while_count < super_runs){
		int index[15], index_over[15], index_till[15], index_tell[15], val = 0, totalOutputBufferCount = 0;
		for(i = 0; i < 15; i++){
			index[i] = val;
			index_over[i] = each;
			index_till[i] = val + each;
			char name[20] = "input.bin.";
			char xx[20];
			sprintf(xx, "%03d", size);
			strcat(name, xx);
			FILE *temp = fopen(name, "r+b");
			fseek(temp, 0L, SEEK_SET);
			int store[each];
			fread(&store, sizeof(int), each, temp);
			index_tell[i] = ftell(temp);
			int j, k = 0;
			for( j = val; j < val + each; j++){
				inputBuffer[j] = store[k];
				k++;
			}
			size++;
			val += each;
			fclose(temp);
		}
			char names[25] = "input.bin.super.";
			char xxs[20];
			sprintf(xxs, "%03d", while_count);
			strcat(names, xxs);
			outputFile = fopen(names, "a+b");
		
		while(1 > 0){
			int min = 2147483647;
			int index_val = 0;
			for( i = 0; i < 15; i++){
				if(((index_over[i] != -1)||(index[i] < index_till[i]))  && (inputBuffer[index[i]] < min)){
					min = inputBuffer[index[i]];
					index_val = i;
				}
			}
			if(min != 2147483647){
				global_count++;
				outputBuffer[totalOutputBufferCount] = min;
				totalOutputBufferCount++;
				index[index_val] += 1;
			}
		
			if((index[index_val] == index_till[index_val]) && index_over[index_val] != -1){
				int sizess = (while_count * 15)+((index[index_val] - 1) / each);
				char namess[20] = "input.bin.";
				char xxss[20];
				sprintf(xxss, "%03d", sizess);
				strcat(namess, xxss);
				FILE *temp = fopen(namess, "r+b");
				fseek(temp, index_tell[index_val], SEEK_SET);
				int count = 0, buff;
				while(fread(&buff, sizeof(int), 1, temp) && count < each){
				inputBuffer[(index[index_val]-each) + count] = buff;
				count++;
			}
				index[index_val] -= each;
				index_till[index_val] -= each;
				index_till[index_val] += count;
				index_over[index_val] += count;
				if(count < each || index_over[index_val] == 1000)	index_over[index_val] = -1;
				else{
					index_tell[index_val] = ftell(temp) - sizeof(int);
				}
				fclose(temp);
			}
			if(totalOutputBufferCount == 1000){
				fwrite(outputBuffer, sizeof(int), totalOutputBufferCount, outputFile );
				totalOutputBufferCount = 0;
			}
			int cou = 0;
			for( i = 0; i < 15; i++){
				if((index_over[i] == -1)&&(index[i] == index_till[i])) cou++;
			}
			if(cou == 15)	break;
		}
		if(totalOutputBufferCount > 0){
			fwrite(outputBuffer, sizeof(int), totalOutputBufferCount, outputFile );
			totalOutputBufferCount = 0;
		}
		fclose(outputFile);
		while_count++;
	}
	// the remaining files
	int index[super_remaining_runs], index_over[super_remaining_runs], index_till[super_remaining_runs], index_tell[super_remaining_runs], val = 0, totalOutputBufferCount = 0;
		for(i = 0; i < super_remaining_runs; i++){
			index[i] = val;
			index_over[i] = each;
			index_till[i] = val + each;
			char name[20] = "input.bin.";
			char xx[20];
			sprintf(xx, "%03d", size);
			strcat(name, xx);
			FILE *temp = fopen(name, "r+b");
			fseek(temp, 0L, SEEK_SET);
			int store[each];
			fread(&store, sizeof(int), each, temp);
			index_tell[i] = ftell(temp);
			int j, k = 0;
			for( j = val; j < val + each; j++){
				inputBuffer[j] = store[k];
				k++;
			}
			size++;
			val += each;
			fclose(temp);
		}
			char names[25] = "input.bin.super.";
			char xxs[20];
			sprintf(xxs, "%03d", while_count);
			strcat(names, xxs);
			outputFile = fopen(names, "a+b");
		
		while(1 > 0){
			int min = 2147483647;
			int index_val = 0;
			for( i = 0; i < super_remaining_runs; i++){
				if(((index_over[i] != -1)||(index[i] < index_till[i]))  && (inputBuffer[index[i]] < min)){
					min = inputBuffer[index[i]];
					index_val = i;
				}
			}
			if(min != 2147483647){
				global_count++;
				outputBuffer[totalOutputBufferCount] = min;
				totalOutputBufferCount++;
				index[index_val] += 1;
			}
		
			if((index[index_val] == index_till[index_val]) && index_over[index_val] != -1){
				int sizess = (while_count * 15)+((index[index_val] - 1) / each);
				char namess[20] = "input.bin.";
				char xxss[20];
				sprintf(xxss, "%03d", sizess);
				strcat(namess, xxss);
				FILE *temp = fopen(namess, "r+b");
				fseek(temp, index_tell[index_val], SEEK_SET);
				int count = 0, buff;
				while(fread(&buff, sizeof(int), 1, temp) && count < each){
				inputBuffer[(index[index_val]-each) + count] = buff;
				count++;
			}
				index[index_val] -= each;
				index_till[index_val] -= each;
				index_till[index_val] += count;
				index_over[index_val] += count;
				if(count < each || index_over[index_val] == 1000)	index_over[index_val] = -1;
				else{
					index_tell[index_val] = ftell(temp) - sizeof(int);
				}
				fclose(temp);
			}
			if(totalOutputBufferCount == 1000){
				fwrite(outputBuffer, sizeof(int), totalOutputBufferCount, outputFile );
				totalOutputBufferCount = 0;
			}
			int cou = 0;
			for( i = 0; i < super_remaining_runs; i++){
				if((index_over[i] == -1)&&(index[i] == index_till[i])) cou++;
			}
			if(cou == super_remaining_runs)	break;
		}
		if(totalOutputBufferCount > 0){
			fwrite(outputBuffer, sizeof(int), totalOutputBufferCount, outputFile );
			totalOutputBufferCount = 0;
		}
		fclose(outputFile);
	return 0;	
}

int basicSortRunFiles15(char *inputFileName, char *outputFileName){
	int i, size = 000, val = 0, totalOutputBufferCount = 0, global_count = 0;
	int outputBuffer[1000];
	FILE *inputFile, *outputFile;
	inputFile = fopen(inputFileName, "r+b");
	outputFile = fopen(outputFileName, "w+b");
	fclose(outputFile);
	outputFile = fopen(outputFileName, "a+b");
	fseek(inputFile, 0L, SEEK_END);
	int sizeKey = ftell(inputFile)/sizeof(int);
	fseek(inputFile, 0L, SEEK_SET);
	int times = (sizeKey/1000)/15;
	if((sizeKey/1000)%15 > 0) times++;
	int inputBuffer[1000];
	int each = 1000/times;
	int index[times], index_over[times], index_till[times], index_tell[times];
	for( i = 0; i < times; i++){
		index[i] = val;
		index_over[i] = each;
		index_till[i] = val + each;
		char name[20] = "input.bin.super.";
		char xx[20];
		sprintf(xx, "%03d", size);
		strcat(name, xx);
		FILE *temp = fopen(name, "r+b");
		fseek(temp, 0L, SEEK_SET);
		int store[each];
		fread(&store, sizeof(int), each, temp);
		index_tell[i] = ftell(temp);
		int j, k = 0;
		for( j = val; j < val + each; j++){
			inputBuffer[j] = store[k];
			k++;
		}
		size++;
		val += each;
		fclose(temp);
	}
	while(1 > 0){
		
		int min = 2147483647;
		int index_val = 0;
		for( i = 0; i < times; i++){
			if(((index_over[i] != -1)||(index[i] < index_till[i]))  && (inputBuffer[index[i]] < min)){
				min = inputBuffer[index[i]];
				index_val = i;
			}
		}
		if(min != 2147483647){
			global_count++;
			outputBuffer[totalOutputBufferCount] = min;
			totalOutputBufferCount++;
			index[index_val] += 1;
		}
		
		if((index[index_val] == index_till[index_val]) && index_over[index_val] != -1){
			int size = (index[index_val] - 1) / each;
			char name[20] = "input.bin.super.";
			char xx[20];
			sprintf(xx, "%03d", size);
			strcat(name, xx);
			FILE *temp = fopen(name, "r+b");
			fseek(temp, 0L, SEEK_END);
			int sizeKeyValue = ftell(temp)/sizeof(int);
			fseek(temp, index_tell[index_val], SEEK_SET);
			int count = 0, buff;
			while(fread(&buff, sizeof(int), 1, temp) && count < each){
				inputBuffer[(index[index_val]-each) + count] = buff;
				count++;
			}
			index[index_val] -= each;
			index_till[index_val] -= each;
			index_till[index_val] += count;
			index_over[index_val] += count;
			if(count < each || index_over[index_val] == sizeKeyValue)	index_over[index_val] = -1;
			else{
				index_tell[index_val] = ftell(temp) - sizeof(int);
			}
			fclose(temp);
		}
		if(totalOutputBufferCount == 1000){
			fwrite(outputBuffer, sizeof(int), totalOutputBufferCount, outputFile );
			totalOutputBufferCount = 0;
		}
		int cou = 0;
		for( i = 0; i < times; i++){
			if((index_over[i] == -1)&&(index[i] == index_till[i])) cou++;
		}
		if(cou == times)	break;
	}
	if(totalOutputBufferCount > 0){
		fwrite(outputBuffer, sizeof(int), totalOutputBufferCount, outputFile );
		totalOutputBufferCount = 0;
	}
	fclose(outputFile);
}

// Replacement Merge

void sift(int *inputBuffer, int index, int totalsize){
	while(index < (totalsize/2)){
		int k = (index + 1) * 2;
		int j = k - 1;
		int lg;
		if( k < totalsize && inputBuffer[k] < inputBuffer[j])	lg = k;
		else lg = j;
		if(inputBuffer[index] <= inputBuffer[lg]){
			break;
		}
		int temp = inputBuffer[index];
		inputBuffer[index] = inputBuffer[lg];
		inputBuffer[lg] = temp;
		index = lg;
	}
}

void heapify(int *inputBuffer, int totalsize){
	int i = totalsize/2 - 1;
	while(i >= 0){
		sift(inputBuffer, i, totalsize);
		i--;
	}
}

int basicSortRunFilesReplacement(char *inputFileName, char *outputFileName, int size_count){
	int i, size = 000, val = 0, totalOutputBufferCount = 0, global_count = 0;
	FILE *outputFile;
	int outputBuffer[1000];
	outputFile = fopen(outputFileName, "w+b");
	fclose(outputFile);
	outputFile = fopen(outputFileName, "a+b");
	int times = size_count + 1;
	int inputBuffer[1000];
	int each = 1000/times;
	int index[times], index_over[times], index_till[times], index_tell[times];
	int sizeKey = 0;
	for( i = 0; i < times; i++){
		index[i] = val;
		index_over[i] = each;
		index_till[i] = val + each;
		char name[20] = "input.bin.";
		char xx[20];
		sprintf(xx, "%03d", size);
		strcat(name, xx);
		FILE *temp = fopen(name, "r+b");
		fseek(temp, 0L, SEEK_SET);
		int store[each];
		fread(&store, sizeof(int), each, temp);
		index_tell[i] = ftell(temp);
		int j, k = 0;
		for( j = val; j < val + each; j++){
			inputBuffer[j] = store[k];
			k++;
		}
		size++;
		val += each;
		fclose(temp);
	}
	while(1 > 0){
		global_count++;
		int min = 2147483647;
		int index_val = 0;
		for( i = 0; i < times; i++){
			if(((index_over[i] != -1)||(index[i] < index_till[i]))  && (inputBuffer[index[i]] < min)){
				min = inputBuffer[index[i]];
				index_val = i;
			}
		}
		if(min != 2147483647){
			outputBuffer[totalOutputBufferCount] = min;
			totalOutputBufferCount++;
			index[index_val] += 1;
		}
		
		if((index[index_val] == index_till[index_val]) && index_over[index_val] != -1){
			int size = (index[index_val] - 1) / each;
			char name[20] = "input.bin.";
			char xx[20];
			sprintf(xx, "%03d", size);
			strcat(name, xx);
			FILE *temp = fopen(name, "r+b");
			fseek(temp, 0L, SEEK_END);
			int sizeKeyValue = ftell(temp)/sizeof(int);
			fseek(temp, index_tell[index_val], SEEK_SET);
			int count = 0, buff;
			while(fread(&buff, sizeof(int), 1, temp) && count < each){
				inputBuffer[(index[index_val]-each) + count] = buff;
				count++;
			}
			index[index_val] -= each;
			index_till[index_val] -= each;
			index_till[index_val] += count;
			index_over[index_val] += count;
			if(count < each || index_over[index_val] == sizeKeyValue)	index_over[index_val] = -1;
			else{
				index_tell[index_val] = ftell(temp) - sizeof(int);
			}
			fclose(temp);
		}
		if(totalOutputBufferCount == 1000){
			fwrite(outputBuffer, sizeof(int), totalOutputBufferCount, outputFile );
			totalOutputBufferCount = 0;
		}
		int cou = 0;
		for( i = 0; i < times; i++){
			if((index_over[i] == -1)&&(index[i] == index_till[i])) cou++;
		}
		if(cou == times)	break;
	}
}

void generateReplacementRunFiles(char *inputFileName, char *outputFileName){
	int inputBuffer[1000], outputBuffer[1000], heapIndex = 0, heapIndexEnd = 749, bufferIndex = 750, outputBufferIndex = 0, heapsize = 750, buffersize = 1000, i;
	int inputFileTell, globalcount = 0, globaldecrease = 250000;
	FILE *inputFile;
	inputFile = fopen(inputFileName, "r+b");
	fseek(inputFile, 0L, SEEK_END);
	int sizeKey = ftell(inputFile)/sizeof(int);
	fseek(inputFile, 0L, SEEK_SET);
	int size = 000;
	if(size == 0){
		char name[20] = "input.bin.";
		char xx[20];
		sprintf(xx, "%03d", 0);
		strcat(name, xx);
		FILE *temp = fopen(name, "w+b");
		fclose(temp);
	}
	fread(&inputBuffer, sizeof(int), 1000, inputFile);
	inputFileTell = ftell(inputFile);
	heapify(inputBuffer, heapsize);
	sizeKey -= 1000;
	while(1 > 0){
		globalcount++;
		if(inputBuffer[heapIndex] <= inputBuffer[bufferIndex]){
			outputBuffer[outputBufferIndex] = inputBuffer[heapIndex];
			outputBufferIndex++;
			globaldecrease--;
			inputBuffer[heapIndex] = inputBuffer[bufferIndex];
			sift(inputBuffer, heapIndex, heapsize);
			bufferIndex++;
			if(sizeKey == 0 && bufferIndex == buffersize){
				char name[20] = "input.bin.";
				char xx[20];
				sprintf(xx, "%03d", size);
				strcat(name, xx);
				FILE *temp = fopen(name, "a+b");
				fwrite(outputBuffer, sizeof(int), outputBufferIndex, temp);
				outputBufferIndex = 0;
				int for_remaining[heapIndexEnd + 1];
				while(heapIndex <= heapIndexEnd ){
					globalcount++;
					for_remaining[heapIndex] = inputBuffer[heapIndex];
					heapIndex++;
					globaldecrease--;
				}
				qsort(&for_remaining, heapIndexEnd + 1, sizeof(int), cmpfuncs);
				int h = 0;
				for(h = 0; h <= heapIndexEnd; h++ ){
					outputBuffer[outputBufferIndex] = for_remaining[h];
					outputBufferIndex++;
				}
				fwrite(outputBuffer, sizeof(int), outputBufferIndex, temp);
				outputBufferIndex = 0;
				fclose(temp);
				size++;
				char snames[20] = "input.bin.";
				char sxxs[20];
				sprintf(sxxs, "%03d", size);
				strcat(snames, sxxs);
				FILE *stemp = fopen(snames, "w+b");
				fclose(stemp);
				int u = 750 - heapIndexEnd - 1;
				int arr[750 - heapIndexEnd - 1];
				for(i = heapIndexEnd + 1; i < 750; i++){
					globalcount++;
					globaldecrease--;
					arr[i - heapIndexEnd - 1] = inputBuffer[i];
				}
				qsort(&arr, 750 - heapIndexEnd - 1, sizeof(int), cmpfuncs);
				char names[20] = "input.bin.";
				char xxs[20];
				sprintf(xxs, "%03d", size);
				strcat(names, xxs);
				FILE *temps = fopen(names, "a+b");
				fwrite(arr, sizeof(int), 750 - heapIndexEnd - 1, temps);
				fclose(temps);
				int file_check_size = 750 - heapIndexEnd - 1;
				if(file_check_size == 0)	size--;
				break;
			}
			if(bufferIndex == buffersize){
				if(sizeKey >= 250){
					fread(&inputBuffer[750], sizeof(int), 250, inputFile);
					bufferIndex = 750;
					sizeKey -= 250;
				}
				else{
					fread(&inputBuffer[750], sizeof(int), sizeKey, inputFile);
					bufferIndex = 750;
					buffersize = 750 + sizeKey - 1;
					sizeKey = 0;
				}
			}
			if(outputBufferIndex == 1000){
				char name[20] = "input.bin.";
				char xx[20];
				sprintf(xx, "%03d", size);
				strcat(name, xx);
				FILE *temp = fopen(name, "a+b");
				fwrite(outputBuffer, sizeof(int), outputBufferIndex, temp);
				fclose(temp);
				outputBufferIndex = 0;
			}
		}
		else{
			if(heapIndexEnd != 0){
				outputBuffer[outputBufferIndex] = inputBuffer[heapIndex];
				outputBufferIndex++;
				globaldecrease--;
				inputBuffer[heapIndex] = inputBuffer[heapIndexEnd];
				inputBuffer[heapIndexEnd] = inputBuffer[bufferIndex];
				heapIndexEnd--;
				heapsize--;
				bufferIndex++;
				if(sizeKey == 0 && bufferIndex == buffersize){
					char name[20] = "input.bin.";
					char xx[20];
					sprintf(xx, "%03d", size);
					strcat(name, xx);
					FILE *temp = fopen(name, "a+b");
					fwrite(outputBuffer, sizeof(int), outputBufferIndex, temp);
					outputBufferIndex = 0;
					int for_remaining[heapIndexEnd + 1];
					while(heapIndex <= heapIndexEnd ){
						globalcount++;
						for_remaining[heapIndex] = inputBuffer[heapIndex];
						heapIndex++;
						globaldecrease--;
					}
					qsort(&for_remaining, heapIndexEnd + 1, sizeof(int), cmpfuncs);
					int h = 0;
					for(h = 0; h <= heapIndexEnd; h++ ){
						outputBuffer[outputBufferIndex] = for_remaining[h];
						outputBufferIndex++;
					}
					fwrite(outputBuffer, sizeof(int), outputBufferIndex, temp);
					outputBufferIndex = 0;
					fclose(temp);
					size++;
					char snames[20] = "input.bin.";
					char sxxs[20];
					sprintf(sxxs, "%03d", size);
					strcat(snames, sxxs);
					FILE *stemp = fopen(snames, "w+b");
					fclose(stemp);
					int arr[750 - heapIndexEnd - 1];
					for(i = heapIndexEnd + 1; i < 750; i++){
						globalcount++;
						globaldecrease--;
						arr[i - heapIndexEnd - 1] = inputBuffer[i];
					}
					qsort(&arr, 750 - heapIndexEnd - 1, sizeof(int), cmpfuncs);
					char names[20] = "input.bin.";
					char xxs[20];
					sprintf(xxs, "%03d", size);
					strcat(names, xxs);
					FILE *temps = fopen(names, "a+b");
					fwrite(arr, sizeof(int), 750 - heapIndexEnd - 1, temps);
					fclose(temps);
					int file_check_size = 750 - heapIndexEnd - 1;
					if(file_check_size == 0)	size--;
					break;
				}
				sift(inputBuffer, heapIndex, heapsize);
				if(bufferIndex == buffersize){
					if(sizeKey >= 250){
						fread(&inputBuffer[750], sizeof(int), 250, inputFile);
						bufferIndex = 750;
						sizeKey -= 250;
					}
					else{
						fread(&inputBuffer[750], sizeof(int), sizeKey, inputFile);
						bufferIndex = 750;
						buffersize = 750 + sizeKey - 1;
						sizeKey = 0;
					}
				}
				if(outputBufferIndex == 1000){
					char name[20] = "input.bin.";	
					char xx[20];
					sprintf(xx, "%03d", size);
					strcat(name, xx);
					FILE *temp = fopen(name, "a+b");
					fwrite(outputBuffer, sizeof(int), outputBufferIndex, temp);
					fclose(temp);
					outputBufferIndex = 0;
				}
			}
			else{
				outputBuffer[outputBufferIndex] = inputBuffer[heapIndex];
				outputBufferIndex++;
				globaldecrease--;
				inputBuffer[heapIndexEnd] = inputBuffer[bufferIndex];
				bufferIndex++;
				if(sizeKey == 0 && bufferIndex == buffersize){
					char name[20] = "input.bin.";
					char xx[20];
					sprintf(xx, "%03d", size);
					strcat(name, xx);
					FILE *temp = fopen(name, "a+b");
					fwrite(outputBuffer, sizeof(int), outputBufferIndex, temp);
					outputBufferIndex = 0;
					int for_remaining[heapIndexEnd + 1];
					while(heapIndex <= heapIndexEnd ){
						globalcount++;
						for_remaining[heapIndex] = inputBuffer[heapIndex];
						heapIndex++;
						globaldecrease--;
					}
					qsort(&for_remaining, heapIndexEnd + 1, sizeof(int), cmpfuncs);
					int h = 0;
					for(h = 0; h <= heapIndexEnd; h++ ){
						outputBuffer[outputBufferIndex] = for_remaining[h];
						outputBufferIndex++;
					}
					fwrite(outputBuffer, sizeof(int), outputBufferIndex, temp);
					outputBufferIndex = 0;
					fclose(temp);
					size++;
					char snames[20] = "input.bin.";
					char sxxs[20];
					sprintf(sxxs, "%03d", size);
					strcat(snames, sxxs);
					FILE *stemp = fopen(snames, "w+b");
					fclose(stemp);
					int arr[750 - heapIndexEnd - 1];
					for(i = heapIndexEnd + 1; i < 750; i++){
						globalcount++;
						globaldecrease--;
						arr[i - heapIndexEnd - 1] = inputBuffer[i];
					}
					qsort(&arr, 750 - heapIndexEnd - 1, sizeof(int), cmpfuncs);
					char names[20] = "input.bin.";
					char xxs[20];
					sprintf(xxs, "%03d", size);
					strcat(names, xxs);
					FILE *temps = fopen(names, "a+b");
					fwrite(arr, sizeof(int), 750 - heapIndexEnd - 1, temps);
					fclose(temps);
					int file_check_size = 750 - heapIndexEnd - 1;
					if(file_check_size == 0)	size--;
					break;
				}
				heapsize = 750;
				heapIndexEnd = 749;
				heapify(inputBuffer, heapsize);
				if(bufferIndex == buffersize){
					if(sizeKey >= 250){
						fread(&inputBuffer[750], sizeof(int), 250, inputFile);
						bufferIndex = 750;
						sizeKey -= 250;
					}
					else{
						fread(&inputBuffer[750], sizeof(int), sizeKey, inputFile);
						bufferIndex = 750;
						buffersize = 750 + sizeKey - 1;
						sizeKey = 0;
					}
				}
				char name[20] = "input.bin.";
				char xx[20];
				sprintf(xx, "%03d", size);
				strcat(name, xx);
				FILE *temp = fopen(name, "a+b");
				fwrite(outputBuffer, sizeof(int), outputBufferIndex, temp);
				fclose(temp);
				outputBufferIndex = 0;
				size++;
				char snames[20] = "input.bin.";
				char sxxs[20];
				sprintf(sxxs, "%03d", size);
				strcat(snames, sxxs);
				FILE *stemp = fopen(snames, "w+b");
				fclose(stemp);
			}
		}
	}
	basicSortRunFilesReplacement(inputFileName, outputFileName, size);
}

void mergeSortBasic(char *inputFileName, char *outputFileName){
	
	generateRunFiles(inputFileName);
	basicSortRunFiles(inputFileName, outputFileName);
}

void mergeSortMultistep(char *inputFileName, char *outputFileName){
	generateRunFiles(inputFileName);
	generateRunFiles15(inputFileName);
	basicSortRunFiles15(inputFileName, outputFileName);
}

void mergeSortReplacement(char *inputFileName, char *outputFileName){
	generateReplacementRunFiles(inputFileName, outputFileName);
}

int main(int argc, char *argv[]){
	char *inputFileName, *outputFileName;
	int *inputBuffer, *outputBuffer;
	inputFileName = argv[2];
	outputFileName = argv[3];
	if(strcmp( argv[1], "--basic") == 0 ){
		gettimeofday( &tm1, NULL );
		mergeSortBasic(inputFileName, outputFileName);
		gettimeofday( &tm2, NULL );
		if(tm2.tv_usec<tm1.tv_usec )	 printf( "Time: %ld.%06ld\n",(tm2.tv_sec-tm1.tv_sec-1),(tm2.tv_usec+1000000-tm1.tv_usec));
		else	printf( "Time: %ld.%06ld\n",tm2.tv_sec-tm1.tv_sec,tm2.tv_usec-tm1.tv_usec );
	}
	else if(strcmp( argv[1], "--multistep") == 0 ){
		gettimeofday( &tm1, NULL );
		mergeSortMultistep(inputFileName, outputFileName);
		gettimeofday( &tm2, NULL );
		if(tm2.tv_usec<tm1.tv_usec )	 printf( "Time: %ld.%06ld\n",(tm2.tv_sec-tm1.tv_sec-1),(tm2.tv_usec+1000000-tm1.tv_usec));
		else	printf( "Time: %ld.%06ld\n",tm2.tv_sec-tm1.tv_sec,tm2.tv_usec-tm1.tv_usec );
	}
	else if(strcmp( argv[1], "--replacement") == 0 ){
		gettimeofday( &tm1, NULL );
		mergeSortReplacement(inputFileName, outputFileName);
		gettimeofday( &tm2, NULL );
		if(tm2.tv_usec<tm1.tv_usec )	 printf( "Time: %ld.%06ld\n",(tm2.tv_sec-tm1.tv_sec-1),(tm2.tv_usec+1000000-tm1.tv_usec));
		else	printf( "Time: %ld.%06ld\n",tm2.tv_sec-tm1.tv_sec,tm2.tv_usec-tm1.tv_usec );
	}
	else	printf("Enter the parameters correctly\n");
	return 0;
}