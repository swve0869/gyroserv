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
#include <chrono>
//#include <gtkmm.h>
//#include "mywindow.h"


using namespace matplot;

#define SERVPORT 8005
#define CLIENTADDR "10.160.0.86"
#define CLIENTPORT 7000  
#define MSGLEN 200

float range_convert(float oldval, float oldmin, float oldmax, float newmin, float newmax){
    return (((oldval - oldmin) * (newmax - newmin)) / (oldmax - oldmin)) + newmin;
}


int main(int argc, char* argv[]){
    
/*     auto app = Gtk::Application::create("org.gtkmm.examples.base");
    return app->make_window_and_run<MyWindow>(argc, argv);  */

    // time tracking 
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    auto procstart = high_resolution_clock::now();
    auto last_render = procstart;

    //pid control
    double kp = 1;
    double ki = .01;
    double kd = 1;

    //udp socket initialization
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

    cliaddr.sin_family    = AF_INET; // IPv4 
    cliaddr.sin_addr.s_addr = INADDR_ANY; 
    cliaddr.sin_port = htons(CLIENTPORT); 
    inet_pton(AF_INET, CLIENTADDR, &cliaddr.sin_addr); 

    // Bind the udp socket with the server address 
    if ( bind(servsock, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    // graphing vars
    std::vector<double> X = linspace(-1, 1);
    std::vector<double> Y = linspace(-1, 1);
    std::vector<double> Z = linspace(-1, 1);
    auto f = figure(true);
    xlim({-1,1});
    ylim({-1,1});
    zlim({-1,1});
    auto ax = f->current_axes();

    while(1){
        const char* message = "#gyrorequest$";

        char rcvbuff[MSGLEN] = { '\0' };
        char sndbuff[MSGLEN] = {'\0'};
        int msg_flag = 0;
        socklen_t len;
        int n; 
        len = sizeof(cliaddr);  //len is value/result 

        // recv IMU data from arduino
        n = recvfrom(servsock, rcvbuff, MSGLEN,  0, ( struct sockaddr *) &cliaddr, &len); 
        if(n > 0) {printf("%d, %s\n",n,rcvbuff);  }

        // send control data to arduino
        sprintf(sndbuff,"%f:%f:%f",kp,ki,kd);
        printf("sndbuf: %s ",sndbuff);
        if(0 < sendto(servsock,sndbuff,MSGLEN,0,(struct sockaddr *) &cliaddr, len)){
            //printf("success\n");
        }
        


        char* xtok = strtok(rcvbuff, ":");
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

        //printf("Server: %f : %f : %f \n", xslope,yslope,zslope);

        auto [X, Y] = meshgrid(linspace(-1, +1, 10), linspace(-1, +1, 10));
        auto Z = transform(X, Y, [&xslope,&yslope](double x, double y) {
            return x*xslope+ y*yslope ;
        }); 

        
        // timed render of positional graphic
        auto current_time = high_resolution_clock::now();
        auto ms_int = duration_cast<milliseconds> (current_time - last_render);
        if (ms_int > std::chrono::milliseconds(100)){
            auto l = surf(X, Y, Z);
            f->draw();
            last_render = high_resolution_clock::now();
        }
    }

    return 0;
}
