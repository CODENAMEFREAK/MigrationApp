#include<stdio.h>
#include<stdlib.h>
#include<libvirt/libvirt.h>

	/**
	To make Remote connections.
	*/
virConnectPtr makeRemoteConnection(char * connectionString)
	{
		virConnectPtr remoteConnection = NULL;
		remoteConnection = virConnectOpen(connectionString);
		return remoteConnection;
	}

	/**
	To get remote node capabilities.
	*/
char * getRemoteCapabilities(virConnectPtr connection)
	{
		if(connection != NULL)
		{
			return virConnectGetCapabilities(connection);
		}
		else
		{
			return NULL;
		}


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
	To get number of defined but inactive domains.
	*/
int getNumberOfInActiveDomains(virConnectPtr connection)
	{
		if(connection!=NULL)
		{
			//virConnectNumOfDefinedDomains returns number of defined but Inactive Domains.
			return virConnectNumOfDefinedDomains(connection);
		}
		else return -1;
	}
/**
	To get list(Names) of defined but inactive domains.
	*/
void getInactiveDomainsID(virConnectPtr connection, char ** domains, int maxNumOfDomains)
	{
		if(connection!=NULL && domains!=NULL)
			virConnectListDefinedDomains(connection,domains,maxNumOfDomains);
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
	To migrate to destination.
	*/
virDomainPtr migrate(virDomainPtr srcDomainPtr, virConnectPtr destinationConnection)
	{
		if(srcDomainPtr!=NULL && destinationConnection!=NULL)
			return virDomainMigrate(srcDomainPtr,destinationConnection,1,NULL,NULL,0);
		else
			return NULL;
	}


int main(int argc, char * argv[])
	{
		virConnectPtr remoteConnection = makeRemoteConnection("qemu+ssh://csuser@10.112.19.129/system");
		if(remoteConnection==NULL)
			{
				printf("Failed to Connect\n");
				return 1;
			}
			else
				{
					/**
					To get Remote Node capabilities.

						char * capab = getRemoteCapabilities(remoteConnection);
						printf("%s",capab);
					*/
					/**
					To get Remote Host Name.

						char * hostName = getRemoteHostName(remoteConnection);
						printf("%s\n",hostName);
					*/

					/**
					To list all the active domains on that host.
					int numOfDomains = getNumberOfActiveDomains(remoteConnection);
					if(numOfDomains==-1)
					{	printf("No Active Domains present");
					}
					else
					{
						printf("There are %d number of active domains\n",numOfDomains);
						//Once identifying how many domains are there create an array to hold it's ids.
						int *domains = (int *)malloc(sizeof(int)*numOfDomains);
						getDomainsID(remoteConnection,domains,numOfDomains);
						for(int i=0;i<numOfDomains;i++)
							printf("%d\n",*(domains+i));

					}
					*/
					/**
					To list all the inactive domains.
					int numOfDomains = getNumberOfInActiveDomains(remoteConnection);
					if(numOfDomains==-1)
					{	printf("No In Active Domains present");
					}
					else
					{
						printf("There are %d number of inactive domains\n",numOfDomains);
						//Once identifying how many domains are there create an array to hold it's ids.
						char **inactiveDomains = (char **)malloc(sizeof(char*)*numOfDomains);
						getInactiveDomainsID(remoteConnection,inactiveDomains,numOfDomains);
						for(int i=0;i<numOfDomains;i++)
							printf("%s\n",*(inactiveDomains+i));

					}
					*/

					/**
					 Getting connection to Active Domain.
					 int numOfDomains = getNumberOfActiveDomains(remoteConnection);
					if(numOfDomains==-1)
					{	printf("No Active Domains present");
					}
					else
					{
						printf("There are %d number of active domains\n",numOfDomains);
						//Once identifying how many domains are there create an array to hold it's ids.
						int *domains = (int *)malloc(sizeof(int)*numOfDomains);
						getDomainsID(remoteConnection,domains,numOfDomains);
						for(int i=0;i<numOfDomains;i++)
							printf("%d\n",*(domains+i));

						virDomainPtr domainPtr = connectToDomainWithID(remoteConnection,domains[0]);

						if(domainPtr!=NULL)
							printf("Got connection to Domain\n");
					}
					*/



					int numOfDomains = getNumberOfActiveDomains(remoteConnection);
					if(numOfDomains==-1)
					{	printf("No Active Domains present");
					}
					else
					{
						printf("There are %d number of active domains\n",numOfDomains);
						//Once identifying how many domains are there create an array to hold it's ids.
						int *domains = (int *)malloc(sizeof(int)*numOfDomains);
						getDomainsID(remoteConnection,domains,numOfDomains);
						for(int i=0;i<numOfDomains;i++)
							printf("%d\n",*(domains+i));

						virDomainPtr domainPtr = connectToDomainWithID(remoteConnection,domains[0]);

						if(domainPtr!=NULL)
							{
							printf("Got connection to Domain\n");
							}
							virConnectPtr remoteConnectToDestination = makeRemoteConnection("qemu+ssh://utkarsh@10.112.19.243/system");
                            if(remoteConnectToDestination !=NULL)
                            {
                                printf("Connection to destination successfull\nInitiating Migration......");
                                virDomainPtr destinationDomainPtr = migrate(domainPtr, remoteConnectToDestination);
								printf("Got destination domain handle:\n");

                            }
                            else
                            {
                                printf("Failed to open connection to destination Node");
                            }
					}



					virConnectClose(remoteConnection);
					//printf("Connection Successful\n");

				}
		return 0;
	}
