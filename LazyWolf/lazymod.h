#ifndef LAZYMOD_H
#define LAZYMOD_H
#include<ncurses.h>
#include<thread>
#include<mutex>
#include<vector>
#include<stdio.h>
#include<unistd.h>
#include<cstring>
#include<condition_variable>

struct modDimensions{
  int y_size, x_size, y, x;
};


class LazyModule{
    protected:
        char buf[1024];
        std::mutex mlock;
        std::condition_variable cv;
        WINDOW* win;
        int x, y, x_size, y_size;
        char name[11];
        bool inFocus = false;
        bool stopped = false;
        bool ready = false;
        std::thread process;

        virtual void program(){
            //will be the program that is ran and stopped when stopped is true;
            wprintw(win,"Base Module loaded\n");
            wrefresh(win);
        }


    public:
        LazyModule(struct modDimensions d){
            win = newwin(d.y_size, d.x_size, d.y, d.x);
            x = d.x;
            y = d.y;
            x_size = d.x_size;
            y_size = d.y_size;
            strncpy(name, "LazyBMod", 10);
        }

        ~LazyModule(){};

        
        virtual void runMod(){
            //Function to be run as thread in the main()
            stopped = false;
            process = std::thread(&LazyModule::program, this);
        }
        virtual void stopMod(){
            stopped = true;
        }

        void setFocus(bool f){
            inFocus = f;
        }

        void resize(struct modDimensions d){
            x = d.x;
            y = d.y;
            x_size = d.x_size;
            y_size = d.y_size;
            wresize(win, y_size, x_size);
            mvwin(win, y, x);
        }

        WINDOW* getwin(){
            return win;
        }

        void redrawMod(){
            redrawwin(win);
            wnoutrefresh(win);
        }

        virtual void refreshMod(){
            wnoutrefresh(win);
        }

        char* getName(){
            return name;
        }

        virtual void sendModInput(char* buffer){
        }
};

class TestMod : public LazyModule{
    protected:
        void program(){
            wprintw(win,"Test banana started\n");
            wrefresh(win);
            while(!stopped){
                char progbuf[1024];
                std::unique_lock<std::mutex> lk(mlock);
                cv.wait(lk, [this]{return ready;});
                strncpy(progbuf, buf, 1024);
                ready = false;
                lk.unlock();
                wprintw(win, ">%s",progbuf);
                if(inFocus) wrefresh(win);
            }
        }
    public:
        TestMod(modDimensions d)
            :LazyModule(d){
            win = newwin(d.y_size, d.x_size, d.y, d.x);
            x = d.x;
            y = d.y;
            x_size = d.x_size;
            y_size = d.y_size;
            scrollok(win, true);
            strncpy(name, "TestMod", 10);
            
        }
        void sendModInput(char* buffer){
            std::unique_lock<std::mutex> lk(mlock);
            strncpy(buf,buffer,1024);
            ready = true;
            mlock.unlock();
            cv.notify_one();

        }
};

class CountMod : public LazyModule{
    protected:
        int thecount = 0;
        
        void program(){
            wprintw(win, "Starting count:\n");
            wrefresh(win);
            while(!stopped){
                wprintw(win, "%d\n",thecount);
                thecount++;
                sleep(1);
                if(inFocus) wrefresh(win);
            }
        }
    public:
        CountMod(modDimensions d)
            :LazyModule(d){
            win = newwin(d.y_size, d.x_size, d.y, d.x);
            x = d.x;
            y = d.y;
            x_size = d.x_size;
            y_size = d.y_size;
            scrollok(win, true);
            strncpy(name, "CountMod", 10);
            
        }

        void runMod(){
            stopped = false;
            process = std::thread(&CountMod::program, this);
        }
        char* getName(){
            return name;
        }
};



#endif