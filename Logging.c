#include<stdio.h>
#include<time.h>
#include<string.h>


char * getTimeStamp()
{
	time_t ltime;
	ltime =time(NULL);
	return asctime(localtime(&ltime));	
}

void logMessage(FILE *logFilePtr, int migrationNumber, char * hostNameFrom,char * hostNameTo, long int timeInSeconds)
{
	char * timestamp = getTimeStamp();
	timestamp[strlen(timestamp)-1]='\0';
	fprintf(logFilePtr,"%s %d %s %s %ld\n",timestamp,migrationNumber,hostNameFrom,hostNameTo,timeInSeconds);
}

void logTestMetaData(FILE *logFilePtr, char * workloadType, char * benchmarkName, char * testConfiguration)
{
	char * timestamp =getTimeStamp();
	timestamp[strlen(timestamp)-1]='\0';
	fprintf(logFilePtr,"WorkLoad : %s\nBenchmark :%s\nTimestamp : %s\nConfiguration : %s\n",workloadType,benchmarkName,timestamp,testConfiguration);	
}
