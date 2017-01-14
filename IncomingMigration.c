#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<string.h>
#include<libvirt/libvirt.h>



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

void getDomainsID(virConnectPtr connection, int * domains, int maxNumOfDomains)
	{
		if(connection!=NULL && domains!=NULL)
			virConnectListDomains(connection,domains,maxNumOfDomains);
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


void printInformation( virTypedParameterPtr  params,  int nparams)
{
    unsigned long long value =0;
    while(1)
    {
        virTypedParamsGetULLong(params,nparams,VIR_DOMAIN_JOB_MEMORY_DIRTY_RATE,&value);
        printf("No. of pages dirtied -> %llu",value);
        sleep(5);
    }
}

int main(int argc,char *argv[])
{
	virConnectPtr remoteNodeA = makeRemoteConnection("qemu+ssh://csuser@10.112.19.129/system");
    //virConnectPtr remoteNodeB = makeRemoteConnection("qemu+ssh://csuser1@10.112.19.227/system");
	if(remoteNodeA==NULL)
			{
				printf("Failed to Connect\n");
				return 1;
			}
	else{
	char * incomingVMFrom;
        char * destinationVMTo;

	//domainToMigrate is the pointer to domain i.e to be migrated.
        virDomainPtr domainToMigrate = NULL;

       //nodeToMigrate is the pointer to node to which VM is to be migrated.
       virConnectPtr nodeToMigrate = NULL;
		
	int numOfDomainsOnA = getNumberOfActiveDomains(remoteNodeA);

			 //i.e. the VM will be migrated From A to B . So incoming to B.
        int *domains = (int *)malloc(sizeof(int)*numOfDomainsOnA);
	getDomainsID(remoteNodeA,domains,numOfDomainsOnA);
        domainToMigrate = connectToDomainWithID(remoteNodeA,domains[0]);
        free(domains);
        incomingVMFrom = getRemoteHostName(remoteNodeA);
        
////////////////////////////
        
	virDomainJobInfo info;
        virTypedParameterPtr  params= NULL;
        int nparams = 0;
        unsigned int flags =0;
        memset(&info,0,sizeof(info));
        int rc = virDomainGetJobStats(domainToMigrate,&info.type,&params,&nparams,flags);		
		if(rc==0)
		{
			printf("Got information");
			//unsigned long long value;
			/**printf("Printing stats @%s\n", getTimeStamp());
            printf("Time elapsed -> %ull\n",info.timeElapsed);
            printf("Time remaining -> %ull\n",info.timeRemaining);
            printf("Data Total -> %ull\n",info.dataTotal);
            printf("Data Processed -> %ull\n",info.dataProcessed);
            printf("Data Remaining -> %ull\n",info.dataRemaining);
            printf("Memory Total -> %ull\n",info.memTotal);
            printf("Memory Processed -> %ull\n",info.memProcessed);
            printf("Memory Remaining -> %ull\n",info.memRemaining);
            printf("File Total -> %ull\n",info.fileTotal);
            printf("File Processed -> %ull\n",info.fileProcessed);
            printf("File Remaining -> %ull\n",info.fileRemaining);
            */
            printInformation(params,nparams);
            
		}
		
			
		
		
	}
}

