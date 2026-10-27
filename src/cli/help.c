#include "cli/help.h"
#include <stdio.h>

void version(){
  printf("Version: 0.0.1");
}

void help(){
  //TODO: make the actual help page here
  printf("yap : yet another project");
}

void print_tree(const char *path){
  //TODO: print the actual project tree
  (void)path;
  printf("Printing the tree");
}
