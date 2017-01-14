#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
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
        int numparams = 0;
        unsigned int flags =0;
        memset(&info,0,sizeof(info));
        int rc = virDomainGetJobStats(domainToMigrate,&info.type,&params,&nparams,flags);
        
        
	
		
		if(rc==0)
		{
			printf("Got information");
			unsigned long long value;
			if(virTypedParamsGetULLong(params,numparams,VIR_DOMAIN_JOB_DOWNTIME,&value)!=1)
			    printf("Downtime = %llu",value);
		}
		
			
		
		
	}
}

