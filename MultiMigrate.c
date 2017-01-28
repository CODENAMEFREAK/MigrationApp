#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<libvirt/libvirt.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "FreakUtil.h"




	/**
	To get remote node HostName.
	*/
char * getRemoteHostName(virConnectPtr connection)
	{
		if(connection !=NULL)
			return virConnectGetHostname(connection);
		else
			return NULL;
	}


/**
    To make Remote connections.
*/
virConnectPtr makeRemoteConnection(char * connectionString)
	{
		virConnectPtr remoteConnection = NULL;
		remoteConnection = virConnectOpen(connectionString);//virConnectOpen is from libvirt.c
		return remoteConnection;
	}

/**
	To get number of active domains.
*/
int getNumberOfActiveDomains(virConnectPtr connection)
	{
		if(connection!=NULL)
		{
			//virConnectNumOfDomains returns number of Active Domains.
			return virConnectNumOfDomains(connection);
		}
		else return -1;
	}
/**
	To connect to Active Domain with ID.
	*/

virDomainPtr connectToDomainWithID(virConnectPtr connection, int domainID)
	{
		virDomainPtr domPtr = NULL;
		if(connection!=NULL)
			domPtr = virDomainLookupByID(connection,domainID);
		return domPtr;
	}

void getDomainsID(virConnectPtr connection, int * domains, int maxNumOfDomains)
	{
		if(connection!=NULL && domains!=NULL)
			virConnectListDomains(connection,domains,maxNumOfDomains);
	}
/**
	To migrate to destination.
	*/
virDomainPtr migrate(virDomainPtr srcDomainPtr, virConnectPtr destinationConnection)
	{
		if(srcDomainPtr!=NULL && destinationConnection!=NULL)
			return virDomainMigrate(srcDomainPtr,destinationConnection,1,NULL,NULL,0);
		else
			return NULL;
	}

char * generateBaseFileName(char * parameters[])
{
	char * fileName=(char *)malloc(sizeof(char));
	//struct stat st ={0};
	//if (stat("./logs", &st) == -1) {
    //mkdir("./logs", 0700);
	//}
	//fileName = "./logs/";
	strcpy(fileName,parameters[1]);
	strcat(fileName,"_");
	strcat(fileName,parameters[2]);
	strcat(fileName,"_");
	strcat(fileName,parameters[3]);
	return fileName;
}

int fileExists(char * fileName,int fileIDNumber)
{
	char * localFileName =(char *)malloc(sizeof(char));;
	char * localAppender=(char *)malloc(sizeof(char));;
	strcpy(localFileName,fileName);
	sprintf(localAppender,"%d",fileIDNumber);
	if(fileIDNumber>0){
		strcat(localFileName,localAppender);
	}	
	FILE * file = fopen(localFileName,"r");
	printf("Trying to make");
	//free(localFileName);
	printf("Trying to make");
	//free(localAppender);
	if(file!=NULL)
	{
		fclose(file);
		return 1;//i.e. file exists
	}
	else 
		return 0;//file do not exists
}

/**
	To initialize Log File
	*/
FILE * initLogFile(char * parameters[])
{	
	int fileIdNumber = 0;
	char * logFileName = generateBaseFileName(parameters);
	char * appender=(char *)malloc(sizeof(char));;;
	while(fileExists(logFileName,fileIdNumber))
	{
		fileIdNumber++;
	}
	if(fileIdNumber>0)
	{
		sprintf(appender,"%d",fileIdNumber);
		strcat(logFileName,appender);
	}
	printf("Trying to make: %s",logFileName);
	
	
	FILE * fp = fopen(logFileName,"w");
	logTestMetaData(fp,parameters[1],parameters[2],parameters[3]);
	printf("Generated log file %s\n",logFileName);
	return fp;
}
	
/**
	To close Log File
	*/
void doneLog(FILE * fp)
{
	fclose(fp);
}

int main(int argc, char* argv[])
{
		FILE *logFilePtr =NULL;	
		logFilePtr = initLogFile(argv);
		virConnectPtr remoteNodeA = makeRemoteConnection("qemu+ssh://csuser@10.112.19.129/system");
		//virConnectPtr remoteNodeB = makeRemoteConnection("qemu+ssh://utkarsh@10.112.19.243/system");
        virConnectPtr remoteNodeB = makeRemoteConnection("qemu+ssh://csuser1@10.112.19.227/system");
		struct timespec begin ,end;
        //clock_t begin,end ;
		if(remoteNodeA==NULL || remoteNodeB==NULL)
			{
				printf("Failed to Connect\n");
				return 1;
			}
			else
				{

				//printf("Got Connection\n");
                char * hostNameFrom;
                char * hostNameTo;
                    
				int numOfMigrations = 4;
				for(int i=1;i<=numOfMigrations;i++)
				{
                    int numOfDomainsOnA = getNumberOfActiveDomains(remoteNodeA);
                    int numOfDomainsOnB = getNumberOfActiveDomains(remoteNodeB);

                    //domainToMigrate is the pointer to domain i.e to be migrated.
                    virDomainPtr domainToMigrate = NULL;

                    //nodeToMigrate is the pointer to node to which VM is to be migrated.
                    virConnectPtr nodeToMigrate = NULL;

                    if(numOfDomainsOnA==1)
                    {
                        //i.e VM to be migrated is on Node A.
                        int *domains = (int *)malloc(sizeof(int)*numOfDomainsOnA);
						getDomainsID(remoteNodeA,domains,numOfDomainsOnA);
                        domainToMigrate = connectToDomainWithID(remoteNodeA,domains[0]);
                        nodeToMigrate = remoteNodeB;
                        free(domains);
                        hostNameFrom = getRemoteHostName(remoteNodeA);
                        hostNameTo = getRemoteHostName( remoteNodeB);
                        
                        //printf("[Migration %d ]\tMigrating from A[csuser@10.112.19.129] to B[csuser1@10.112.19.227]\n",i);
                    }
                    else
                    {
                        //i.e VM to be migrated is on Node B.
                        int *domains = (int *)malloc(sizeof(int)*numOfDomainsOnB);
						getDomainsID(remoteNodeB,domains,numOfDomainsOnB);
                        domainToMigrate = connectToDomainWithID(remoteNodeB,domains[0]);
                        nodeToMigrate = remoteNodeA;
                        free(domains);
                        hostNameFrom = getRemoteHostName(remoteNodeB);
                        hostNameTo = getRemoteHostName( remoteNodeA);                        
                      //  printf("[Migration %d ]\tMigrating from B[csuser1@10.112.19.227] to A[csuser@10.112.19.129]\n",i);
                    }
					
                    //begin = clock();
					clock_gettime(CLOCK_MONOTONIC_RAW,&begin);	
                    migrate(domainToMigrate,nodeToMigrate);
					clock_gettime(CLOCK_MONOTONIC_RAW,&end);
                    //end = clock();
                    //printf("------------------------DONE------------------------\n\t\tTime Taken: %ld\n",(((end-begin))));
                    
					logMessage(logFilePtr,i,hostNameFrom,hostNameTo,(end.tv_sec-begin.tv_sec) );
                    printf("%d %s  %s  %ld \n",i,hostNameFrom,hostNameTo,(end.tv_sec-begin.tv_sec));
					printf("------------------------DONE------------------------\n\t\tTime Taken(in seconds): %ld\n",(((end.tv_sec-begin.tv_sec))));		
                    //Delay of 15 seconds
                    sleep(15);
				}


					virConnectClose(remoteNodeA);
                    virConnectClose(remoteNodeB);
					doneLog(logFilePtr);
					//printf("Connection Successful\n");
                }
				
		return 0;

}
