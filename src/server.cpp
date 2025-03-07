#include <stdio.h> // standard input and output library
#include <bits/stdc++.h> 
#include <stdlib.h> // this includes functions regarding memory allocation
#include <string.h> // contains string functions
#include <errno.h> //It defines macros for reporting and retrieving error conditions through error codes
#include <time.h> //contains various functions for manipulating date and time
#include <unistd.h> //contains various constants
#include <sys/types.h> //contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h> // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses
#include <cmath>
#include <matplot/matplot.h>
#include <random>

using namespace matplot;

#define SERVPORT 8000
#define CLIENTPORT 80 

float range_convert(float oldval, float oldmin, float oldmax, float newmin, float newmax){
    return (((oldval - oldmin) * (newmax - newmin)) / (oldmax - oldmin)) + newmin;
}

int main(){
    printf("startung udp");
    struct sockaddr_in servaddr,cliaddr;
    int servsock;

    if ( (servsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 

    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 

    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(SERVPORT); 

    // Bind the socket with the server address 
    if ( bind(servsock, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 


    printf("starting server\n");


    std::vector<double> X = linspace(-1, 1);
    std::vector<double> Y = linspace(-1, 1);
    std::vector<double> Z = linspace(-1, 1);
    auto f = figure(true);
    //surf(X, Y, Z);
    xlim({-1,1});
    ylim({-1,1});
    zlim({-1,1});
    auto ax = f->current_axes();

    while(1){
        const char* message = "#gyrorequest$";
        printf("a");
        send(clientSocket, message, strlen(message), 0);
        char buffer[1024] = { 0 };
        printf("h");
        int msg_flag = 0;
        while(1){
            char c = '\0';
            recv(clientSocket, &c, 1, 0);
            if(c == '#'){ continue; }
            if(c == '$'){ break; }
            strncat(buffer, &c, 1);
        }
        //send(clientSocket, message, strlen(message), 0);
        //sleep(1);
        //printf("Server: %s\n", buffer);
        
        char* xtok = strtok(buffer, ":");
        char* ytok = strtok(nullptr, ":");
        char* ztok = strtok(nullptr, ":");

        float xslope = std::stof(xtok);
        float yslope = std::stof(ytok);
        float zslope = std::stof(ztok);

        if(xslope >1 || xslope < -1){trunc(xslope);}
        if(yslope >1 || yslope < -1){trunc(yslope);}

        if(xslope >= 0){ xslope = range_convert(xslope,0,1,0,90);}
        else{ xslope = range_convert(xslope,0,-1,0,-90);}
        if(yslope >= 0){ yslope = range_convert(yslope,0,1,0,90);}
        else{ yslope = range_convert(yslope,0,-1,0,-90);}
        
        float pi = 3.1415;

        xslope = tan(xslope*pi/180);
        yslope = tan(yslope*pi/180); 

        printf("Server: %f : %f : %f \n", xslope,yslope,zslope);

        auto [X, Y] = meshgrid(linspace(-1, +1, 10), linspace(-1, +1, 10));
        auto Z = transform(X, Y, [&xslope,&yslope](double x, double y) {
            return x*xslope+ y*yslope ;
        }); 

        //hold(on);
        //cla();
        auto l = surf(X, Y, Z);
        f->draw();
    }

    return 0;
}
