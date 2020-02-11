#ifndef LAZYWOLF_H
#define LAZYWOLF_H

#include<ncurses.h>
#include"lazymod.h"

struct lwItems{
    std::vector<LazyModule*>* modules;
    WINDOW* header;
    WINDOW* input;
    WINDOW* status;
    int* modfocus;
};

void handle_signal(int signal){
    if (signal == SIGINT){
      endwin();
      printf("Closing Nicely\n");
      exit(0);	
    }
}

WINDOW *init_header(int ysize, int xsize, int y, int x){
    WINDOW *win = newwin(ysize , xsize, y, x);
    wattron(win,A_STANDOUT);
    wmove(win,0,0);
    for(int i = 0; i < xsize*2; i++){
       wprintw(win," ");
    }

    mvwprintw(win,0,0,"LazyWolf Modules Manager");
    wattroff(win, A_STANDOUT);
    wnoutrefresh(win);
    return win;
}

void updateHeader(WINDOW* win, std::vector<LazyModule*> list, int focus){
    int x, y;
    getmaxyx(win, y, x);
    wattron(win,A_STANDOUT);
    wmove(win,0,0);
    for(int i = 0; i < x*2; i++){
       wprintw(win," ");
    }

    mvwprintw(win,0,0,"LazyWolf Modules Manager");
    wmove(win, 1, 0);

    int width = x/list.size();
    for(int i = 0; i < list.size(); i++){
      int pos = ((width/2)-5) + width*i;
      if(i == focus) wattroff(win, A_STANDOUT);
      mvwprintw(win, 1, pos, "%s",list[i]->getName());
      wattron(win,A_STANDOUT);
    }
    wattroff(win, A_STANDOUT);
    //wnoutrefresh(win);
    wrefresh(win);
}

WINDOW *init_input(int ysize, int xsize, int y, int x){
    WINDOW *win = newwin(ysize , xsize, y, x);
    return win;
}

WINDOW *init_status(int ysize, int xsize, int y, int x){
    WINDOW *win = newwin(ysize , xsize, y, x);
    wattron(win,A_STANDOUT);
    wmove(win,0,0);
    for(int i = 0; i < xsize; i++){
       wprintw(win," ");
    }
    mvwprintw(win,0,0,"LazyWolf started!");
    wnoutrefresh(win);
    return win;
}

void updateStatus(WINDOW* win){
    int line_x, line_y, win_x, win_y;
    getmaxyx(win, win_y, win_x);
    getyx(win, line_y, line_x);
    int pos = (win_x/2)-(line_x/2);
    wmove(win,0,0);
    for(int i = 0; i < 1; i++){
        wprintw(win, " ");
    }
    wnoutrefresh(win);
}

void redrawStatus(WINDOW* win){
    int y, x;
    getmaxyx(win, y, x);
    wmove(win,0,0);
    for(int i = 0; i < x; i++){
       wprintw(win," ");
    }
    wnoutrefresh(win);
}

void statusPrint(WINDOW* win, const char* message){
    int win_x, win_y;
    getmaxyx(win, win_y, win_x);
    int pos = (win_x/2)-(strlen(message)/2);
    wmove(win,0,0);
    for(int i = 0; i < pos; i++){
        wprintw(win, " ");
    }
    wprintw(win,"%s",message);
    for(int i = 0; i < win_x-pos-strlen(message); i++){
        wprintw(win, " ");
    }
    wnoutrefresh(win);
}

int changeFocus(std::vector<LazyModule*> list, int cfocus, int newfocus){
    list[cfocus]->setFocus(false);
    if(newfocus >= list.size()){
        newfocus = 0;
    }else if(newfocus < 0){
        newfocus = list.size()-1;
    }
    list[newfocus]->setFocus(true);
    list[newfocus]->redrawMod();

    return newfocus;
}

void loadModual(LazyModule* mod, struct lwItems stuff){
    char buf[100];
    std::vector<LazyModule*> temp = *stuff.modules;

    stuff.modules->push_back(mod);
    mod->runMod();
    mod->refreshMod();
    *stuff.modfocus = changeFocus(*stuff.modules, *stuff.modfocus, stuff.modules->size()+1);

    sprintf(buf, "%s module loaded", mod->getName());
    statusPrint(stuff.status, buf);
}



static void readline(WINDOW* win, char *buffer, int buflen)
/* Read up to buflen-1 characters into `buffer`.
 * A terminating '\0' character is added after the input.  */
{
  int old_curs = curs_set(1);
  int pos = 0;
  int len = 0;
  int x, y;

  getyx(win, y, x);
  for (;;) {
    int c;

    buffer[len] = ' ';
    mvwaddnstr(win, y, x, buffer, len+1);
    wmove(win,y, x+pos);
    wrefresh(win);
    c = wgetch(win);

    if(c == KEY_RESIZE){
      clear();
      buffer[0] = '\0';
      break;
    }

    if (c == KEY_ENTER || c == '\n' || c == '\r') {
      break;
    } else if (isprint(c)) {
      if (pos < buflen-1) {
        memmove(buffer+pos+1, buffer+pos, len-pos);
        buffer[pos++] = c;
        len += 1;
      } else {
        beep();
      }
    } else if (c == KEY_LEFT) {
      if (pos > 0) pos -= 1; else beep();
    } else if (c == KEY_RIGHT) {
      if (pos < len) pos += 1; else beep();
    } else if (c == KEY_BACKSPACE) {
      if (pos > 0) {
        memmove(buffer+pos-1, buffer+pos, len-pos);
        pos -= 1;
        len -= 1;
      } else {
        beep();
      }
    } else if (c == KEY_DC) {
      if (pos < len) {
        memmove(buffer+pos, buffer+pos+1, len-pos-1);
        len -= 1;
      } else {
        beep();
      }
    } else {
      beep();
    }
  }
  buffer[len] = '\0';
  if (old_curs != ERR) curs_set(old_curs);
}

#endif