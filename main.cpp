#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<unistd.h>

#include<sstream>
#include<cstring>
#include<iostream>
#include<vector>
#include<mutex>
#include<thread>

#include<stdlib.h>
#include<netdb.h>
#include<ncurses.h>
#include<signal.h>

#include"lazywolf.h"
#include"lazymod.h"
#include"Modules/LurkWolfMod.h"


int main(){
    signal(SIGINT,handle_signal);
    std::vector<LazyModule*> modules;
    int modfocus = 0;
    char buf[1024];
    int max_y, max_x;
    
    initscr();
    refresh();
    cbreak();
    noecho();
    curs_set(1);
    
    getmaxyx(stdscr,max_y,max_x);
    struct modDimensions modxy = {max_y-4, max_x, 2, 0};
    WINDOW *header = init_header(2,max_x, 0, 0);
    WINDOW *input = init_input(1, max_x, max_y-1, 0);
    WINDOW *status = init_status(1, max_x, max_y-2, 0);
    keypad(input, TRUE);

    struct lwItems items = {&modules, header, input, status, &modfocus};

    loadModual(new LazyModule(modxy), items);
    updateHeader(header, modules, modfocus);
    
    /*MAIN LOOP*/
    for(;;){
        doupdate();
        wclear(input);
        readline(input, buf, 1024);
        redrawStatus(status);
        if(buf[0] == ':'){
            if(!strcmp(buf,":n")){
                modfocus = changeFocus(modules, modfocus, modfocus+1);
            }else if(!strcmp(buf,":b")){
                modfocus = changeFocus(modules, modfocus, modfocus-1);

            }else if(!strcmp(buf,":close")){
                if(modules.size()>1){
                    char temp[100];
                    int stopindex = modfocus;
                    sprintf(temp, "%s module closed", modules[stopindex]->getName());
                    statusPrint(status, temp);
                    modules[modfocus]->stopMod();
                    modfocus = changeFocus(modules, modfocus, modfocus-1);
                    modules.erase(modules.begin() + stopindex);
                    
                    
                }

            }else if(!strcmp(buf,":count")){
                loadModual(new CountMod(modxy), items);
                modfocus = changeFocus(modules, modfocus, modules.size()-1);
                

            }else if(!strcmp(buf,":test")){
                loadModual(new TestMod(modxy), items);
                modfocus = changeFocus(modules, modfocus, modules.size()-1);
            }else if(!strcmp(buf,":lurk")){
                loadModual(new LurkWolf(modxy), items);
                modfocus = changeFocus(modules, modfocus, modules.size()-1);
            }
        }else if(buf[0]=='\0'){
          getmaxyx(stdscr,max_y, max_x);
          modxy = {max_y-4, max_x, 2, 0};
          mvwin(input,max_y-1, 0);
          mvwin(status,max_y-2, 0);
          for(int i = 0; i < modules.size(); i++){
              modules[i]->resize(modxy);
          }
        }else{
            modules[modfocus]->sendModInput(buf);
        }

        modules[modfocus]->refreshMod();
        updateHeader(header, modules, modfocus);
        
        

    }
    /*END MAIN LOOP*/
    endwin();
    return 0;
}



