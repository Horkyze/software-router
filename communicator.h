#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUFLEN 1024  //Max length of buffer


void * listener(void * arg)
{
    struct sockaddr_in si_me, si_other;

    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        my_log("cant socket");
        return 0;
    }

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(5000);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        my_log("cant bind");
        return 0;
    }

    //keep listening for data
    while(1)
    {
        my_log("Waiting for data...");
        fflush(stdout);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, (socklen_t *) &slen)) == -1)
        {
            my_log("cant recvfrom");
            return 0;
        }

        //print details of the client/peer and the data received
        sprintf(log_b, "Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        my_log(log_b);

        sprintf(log_b, "Data: %s\n" , buf);
        my_log(log_b);

        // executes cmd and fills response string
        parse_command(buf);

        //now reply the client with the same data
        if (sendto(s, response, 1024, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            my_log("cant sendto");
            return 0;
        }
        strclr(response);
    }

    close(s);
}
