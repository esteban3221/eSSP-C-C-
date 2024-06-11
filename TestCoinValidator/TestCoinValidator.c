#include "../inc/SSPComs.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

unsigned deno[7] = {0,20,50,100,200,500,1000};

void ParsePoll(SSP_POLL_DATA * poll)
{
	unsigned long a;
	for (int i = 0; i < poll->event_count; ++i)
	{
		switch(poll->events[i].event)
		{
		case SSP_POLL_RESET:
			printf("Unit Reset\n");
			break;
		case SSP_POLL_READ:
			if (poll->events[i].data > 0)
			a = poll->events[i].data;
			break;
		case SSP_POLL_CREDIT:
			a = poll->events[i].data;
			break;
		case SSP_POLL_STACKING:
			break;
		case SSP_POLL_STACKED:
		{
			printf("Stacked %d MXN$\n" , deno[a]);
			a = 0;
			break;
		}
		case SSP_POLL_FRAUD_ATTEMPT:
			printf("Fraud Attempt %ld\n",poll->events[i].data);
			break;
		case SSP_POLL_STACKER_FULL:
			printf("Stacker Full\n");
			break;
		}
	}
}

void RunValidator(SSP_PORT port, const unsigned char ssp_address)
{
    SSP_COMMAND_SETUP ssp_setup;
    SSP_POLL_DATA poll;

	ssp_setup.port = port;
	ssp_setup.Timeout = 1000;
	ssp_setup.RetryLevel = 3;
	ssp_setup.SSPAddress = ssp_address;
	ssp_setup.EncryptionStatus = NO_ENCRYPTION;

	if (ssp_setup_encryption(&ssp_setup,(unsigned long long)0x123456701234567LL) != SSP_RESPONSE_OK)

	ssp_enable(ssp_setup);
	ssp_set_inhibits(ssp_setup,0xFF,0xFF);

	while (1)
	{
	    if (ssp_poll(ssp_setup,&poll) != SSP_RESPONSE_OK)
        {
            printf("SSP_POLL_ERROR\n");
        }
	    ParsePoll(&poll);
        usleep(50000); //500 ms delay between polls
	}
	
}

int main(int argc, char *argv[])
{
    int ssp_address;
	SSP_PORT port;


    //check for command line arguments - first is port, second is ssp address
	if (argc <= 2)
	{
	    printf("Usage: BasicValidator <port> <sspaddress>\n");
	    return 1;
    }

    //open the com port
    printf("PORT: %s\n",argv[1]);
    port = OpenSSPPort(argv[1]);
    if (port == -1)
	{
	    printf("Port Error\n");
        return 1;
	}

    //set the ssp address
    ssp_address = atoi(argv[2]);
    printf("SSP ADDRESS: %d\n",ssp_address);


    //run the validator
	RunValidator(port,ssp_address);
	CloseSSPPort(port);
	return 0;


	//printf("%x\n",DownloadFileToTarget("/home/rstacey/CNY01609_NV02004000000TES_IF_01.bv1",PORT_TO_USE,0));

}
