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

int main(int argc, char* argv[])
{

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

				printf("Got Connection\n");
				int numOfMigrations = 10;
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
                        printf("[Migration %d ]\tMigrating from A[csuser@10.112.19.129] to B[csuser1@10.112.19.227]\n",i);
                    }
                    else
                    {
                        //i.e VM to be migrated is on Node B.
                        int *domains = (int *)malloc(sizeof(int)*numOfDomainsOnB);
						getDomainsID(remoteNodeB,domains,numOfDomainsOnB);
                        domainToMigrate = connectToDomainWithID(remoteNodeB,domains[0]);
                        nodeToMigrate = remoteNodeA;
                        free(domains);
                        printf("[Migration %d ]\tMigrating from B[csuser1@10.112.19.227] to A[csuser@10.112.19.129]\n",i);
                    }
					
                    //begin = clock();
					clock_gettime(CLOCK_MONOTONIC_RAW,&begin);	
                    migrate(domainToMigrate,nodeToMigrate);
					clock_gettime(CLOCK_MONOTONIC_RAW,&end);
                    //end = clock();
                    //printf("------------------------DONE------------------------\n\t\tTime Taken: %ld\n",(((end-begin))));
					printf("------------------------DONE------------------------\n\t\tTime Taken(in seconds): %ld\n",(((end.tv_sec-begin.tv_sec))));		
                    //Delay of 15 seconds
                    sleep(15);
				}


					virConnectClose(remoteNodeA);
                    virConnectClose(remoteNodeB);
					//printf("Connection Successful\n");
                }
				
		return 0;

}
