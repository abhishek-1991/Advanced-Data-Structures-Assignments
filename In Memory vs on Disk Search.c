#include<stdio.h>
#include<string.h>

#include<sys/time.h>

void inMemoryLinearSearch (char*, char*);
void inMemoryBinarySearch (char*, char*);
void onDiskLinearSearch (char*, char*);
void onDiskBinarySearch (char*, char*);
void printResult(int*,int*,int,struct timeval);
int binarySearchArr(int*,long,long,int);
int binarySearchFile(FILE*,long,long,int);


void inMemoryLinearSearch(char* key, char* seek)
{
	FILE *seekdb,*keydb;
	struct timeval tmStart,tmEnd,tmDiff;
	long seeklen,keylen,i,j;
	
	//Reading seek.db into the memory
	seekdb = fopen(seek,"rb");
	if(!seekdb)
	{
		fprintf(stderr,"ERROR:\nUnable to open %s\n",seek);
		return;
	}
	fseek(seekdb,0,SEEK_END);
	seeklen=ftell(seekdb);
	seeklen /= sizeof(int);
	fseek(seekdb,0,SEEK_SET);
	
	int seekarr[seeklen],hit[seeklen];
	
	for(i=0;i<seeklen;i++)
	{
		fseek(seekdb,i*sizeof(int),SEEK_SET);
		fread(&seekarr[i],sizeof(int),1,seekdb);
	}
	fclose(seekdb);
	
	//Initializing hit array
	for(i=0;i<seeklen;i++)
		hit[i]=0;
	
	gettimeofday(&tmStart,NULL);
	
	//Reading key.db into the memory
	keydb = fopen(key,"rb");
	if(!keydb)
	{
		fprintf(stderr,"ERROR:\nUnable to open %s\n",key);
		return;
	}
	fseek(keydb,0,SEEK_END);
	keylen=ftell(keydb);
	keylen /= sizeof(int);
	fseek(keydb,0,SEEK_SET);
	
	int keyarr[keylen];
	
	for(i=0;i<keylen;i++)
	{
		fseek(keydb,i*sizeof(int),SEEK_SET);
		fread(&keyarr[i],sizeof(int),1,keydb);
	}
	
	//Linear Search
	for(i=0;i<seeklen;i++)
	{
		for(j=0;j<keylen;j++)
		{
			if(seekarr[i] == keyarr[j])
				hit[i] = 1;
		}
	}
	
	gettimeofday(&tmEnd,NULL);
	
	fclose(keydb);
	
	//Total time for the whole operation
	timersub(&tmEnd,&tmStart,&tmDiff);
	
	printResult(seekarr,hit,seeklen,tmDiff);
	
}

void inMemoryBinarySearch (char* key, char* seek)
{
	FILE *seekdb,*keydb;
	struct timeval tmStart,tmEnd,tmDiff;
	long seeklen,keylen,i;
	
	//Reading seek.db into the memory
	seekdb = fopen(seek,"rb");
	if(!seekdb)
	{
		fprintf(stderr,"ERROR:\nUnable to open %s\n",seek);
		return;
	}
	fseek(seekdb,0,SEEK_END);
	seeklen=ftell(seekdb);
	seeklen /= sizeof(int);
	fseek(seekdb,0,SEEK_SET);
	
	int seekarr[seeklen],hit[seeklen];
	
	for(i=0;i<seeklen;i++)
	{
		fseek(seekdb,i*sizeof(int),SEEK_SET);
		fread(&seekarr[i],sizeof(int),1,seekdb);
	}
	fclose(seekdb);
	
	//Initializing hit array
	for(i=0;i<seeklen;i++)
		hit[i]=0;
	
	gettimeofday(&tmStart,NULL);
	
	//Reading key.db into the memory
	keydb = fopen(key,"rb");
	if(!keydb)
	{
		fprintf(stderr,"ERROR:\nUnable to open %s\n",key);
		return;
	}
	fseek(keydb,0,SEEK_END);
	keylen=ftell(keydb);
	keylen /= sizeof(int);
	fseek(keydb,0,SEEK_SET);
	
	int keyarr[keylen];
	for(i=0;i<keylen;i++)
	{
		fseek(keydb,i*sizeof(int),SEEK_SET);
		fread(&keyarr[i],sizeof(int),1,keydb);
	}
	
	for(i=0;i<seeklen;i++)
	{		
		if(binarySearchArr(keyarr,0,keylen-1,seekarr[i]))
			hit[i]=1;
	}
	
	gettimeofday(&tmEnd,NULL);
	
	fclose(keydb);
	
	//Total time for the whole operation
	timersub(&tmEnd,&tmStart,&tmDiff);
	
	printResult(seekarr,hit,seeklen,tmDiff);

}

void onDiskLinearSearch (char* key, char* seek)
{
	FILE *seekdb,*keydb;
	struct timeval tmStart,tmEnd,tmDiff;
	long seeklen,i;
	
	//Reading seek.db into the memory
	seekdb = fopen(seek,"rb");
	if(!seekdb)
	{
		fprintf(stderr,"ERROR:\nUnable to open %s\n",seek);
		return;
	}
	fseek(seekdb,0,SEEK_END);
	seeklen=ftell(seekdb);
	seeklen /= sizeof(int);
	fseek(seekdb,0,SEEK_SET);
	
	int seekarr[seeklen],hit[seeklen];
	
	for(i=0;i<seeklen;i++)
	{
		fseek(seekdb,i*sizeof(int),SEEK_SET);
		fread(&seekarr[i],sizeof(int),1,seekdb);
	}
	
	fclose(seekdb);
	
	//Initializing hit array
	for(i=0;i<seeklen;i++)
		hit[i]=0;
	
	gettimeofday(&tmStart,NULL);
	
	//Reading key.db into the memory
	keydb = fopen(key,"rb");
	if(!keydb)
	{
		fprintf(stderr,"ERROR:\nUnable to open %s\n",key);
		return;
	}
	
	//Linear Search for each element in seekarr[]
	for (i = 0; i < seeklen; i++)
	{
		fseek(keydb, 0, SEEK_SET);
		clearerr(keydb);
		while(-1)
		{
			int k;
			int ret=fread(&k, sizeof(int), 1, keydb);
			if(ret==0)
			{
				break;
			}
			if (seekarr[i] == k)
			{
				hit[i] = 1;
				break;
			}
		}		
	}
	
	gettimeofday(&tmEnd,NULL);
	
	fclose(keydb);
	
	//Total time for the whole operation
	timersub(&tmEnd,&tmStart,&tmDiff);
	
	printResult(seekarr,hit,seeklen,tmDiff);
}

void onDiskBinarySearch (char* key, char* seek)
{
	FILE *seekdb,*keydb;
	struct timeval tmStart,tmEnd,tmDiff;
	long seeklen,keylen,i;
	
	//Reading seek.db into the memory
	seekdb = fopen(seek,"rb");
	if(!seekdb)
	{
		fprintf(stderr,"ERROR:\nUnable to open %s\n",seek);
		return;
	}
	fseek(seekdb,0,SEEK_END);
	seeklen=ftell(seekdb);
	seeklen /= sizeof(int);
	fseek(seekdb,0,SEEK_SET);
	
	int seekarr[seeklen],hit[seeklen];
	
	for(i=0;i<seeklen;i++)
	{
		fseek(seekdb,i*sizeof(int),SEEK_SET);
		fread(&seekarr[i],sizeof(int),1,seekdb);
	}
	
	fclose(seekdb);
	
	//Initializing hit array
	for(i=0;i<seeklen;i++)
		hit[i]=0;
	
	gettimeofday(&tmStart,NULL);
	//Reading key.db into the memory
	keydb = fopen(key,"rb");
	if(!keydb)
	{
		fprintf(stderr,"ERROR:\nUnable to open %s\n",key);
		return;
	}
	fseek(keydb,0,SEEK_END);
	keylen=ftell(keydb);
	keylen /= sizeof(int);
	fseek(keydb,0,SEEK_SET);
	
	for(i=0;i<seeklen;i++)
	{
		if(binarySearchFile(keydb,0,keylen-1,seekarr[i]))
			hit[i]=1;
	}
	
	gettimeofday(&tmEnd,NULL);
	
	fclose(keydb);
	
	//Total time for the whole operation
	timersub(&tmEnd,&tmStart,&tmDiff);
	
	printResult(seekarr,hit,seeklen,tmDiff);
}

void printResult(int* seek,int* hit,int length,struct timeval tm)
{
	long i;
	for(i=0;i<length;i++)
	{
		if(hit[i] == 1)
			printf("%12d: Yes\n",seek[i]);
		else
			printf("%12d: No\n",seek[i]);
	}
	printf( "Time: %ld.%06ld\n", tm.tv_sec, tm.tv_usec );
	
}

//To perform binary search on array
int binarySearchArr(int* keyarr,long l,long r,int val)
{
	if(l > r)
		return 0;
		
	long m = (l+r)/2;
	
	if(keyarr[m] == val)
		return 1;
	
	if(keyarr[m] > val)
		return binarySearchArr(keyarr,l,m-1,val);
	
	if(keyarr[m] < val)
		return binarySearchArr(keyarr,m+1,r,val);
		
	return 0;
}

//To perform binary search in File on disk
int binarySearchFile(FILE* keydb,long l,long r,int val)
{
	if(l > r)
		return 0;
		
	long m = (l+r)/2;
	int k;
	
	fseek(keydb,m*sizeof(int),SEEK_SET);
	fread(&k,sizeof(int),1,keydb);
	
	if(k == val)
		return 1;
	
	if(k > val)
		return binarySearchFile(keydb,l,m-1,val);
	
	if(k < val)
		return binarySearchFile(keydb,m+1,r,val);
		
	return 0;
}

int main(int argc, char* argv[])
{
	if(argc != 4)
	{
		fprintf(stderr,"Execution Format: \nassn_1 <mode> <key_file> <seek_file>\n");
	}
	
	if(!strcmp(argv[1],"--mem-lin"))
		inMemoryLinearSearch(argv[2],argv[3]);
	else if(!strcmp(argv[1],"--mem-bin"))
		inMemoryBinarySearch(argv[2],argv[3]);
	else if(!strcmp(argv[1],"--disk-lin"))
		onDiskLinearSearch(argv[2],argv[3]);
	else if(!strcmp(argv[1],"--disk-bin"))
		onDiskBinarySearch(argv[2],argv[3]);
	else
		fprintf(stderr,"ERROR:\nWrong Search Mode\n");
		
	return 0;
}
