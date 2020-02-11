#include"../lazymod.h"
#include<ncurses.h>
#include<thread>
#include<mutex>
#include<vector>
#include<stdio.h>
#include<unistd.h>
#include<cstring>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<netdb.h>


class LurkWolf : public LazyModule{
    protected:
        std::mutex mlock;
        char buf[1024];

        WINDOW *header;
	    WINDOW *log;
	    WINDOW *pstatus;
        WINDOW *extrawindow1;
        WINDOW *extrawindow2;


        void program(){
            header = newwin(2,x_size,2,0);
	        log = newwin(y_size-6, x_size, 4, 0);
	        pstatus = newwin(2,x_size,y_size-4,0);

            int log_x, log_y;
            getmaxyx(log, log_y, log_x);
            extrawindow1 = newwin(log_y/2-1, 35, 2, x_size-35);
	        extrawindow2 = newwin(log_y/2+(log_y%2)+1, 35, log_y/2+1, x_size-35);
            scrollok(log, true);

            updateWindows();
            mvwprintw(log, log_y, 0, "LurkWolf says Hi!");
            while(!stopped){
                if(buf[0] != 0){
                    wprintw(log, ">%s",buf);
                    buf[0] = 0;
                    if(inFocus)updateWindows();
                }
            }

        }

        void updateWindows(){
            wrefresh(header);
            wrefresh(log);
            wrefresh(pstatus);
            wrefresh(extrawindow1);
            wrefresh(extrawindow2);
        }
        /*
        update_header(){
            
        }
        update_pstatus(){

        }
        update_extrawin(){

        }
        */
    public:
        LurkWolf(modDimensions d)
            :LazyModule(d){
            win = newwin(d.y_size, d.x_size, d.y, d.x);
            x = d.x;
            y = d.y;
            x_size = d.x_size;
            y_size = d.y_size;
            scrollok(win, true);
            strncpy(name, "LurkWolf", 10);
            
        }

        void sendModInput(char* buffer){
            mlock.lock();
            strcpy(buf, buffer);
            mlock.unlock();
        }
        char* getName(){
            return name;
        }
};