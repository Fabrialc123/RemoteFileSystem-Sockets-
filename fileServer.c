#include "fileServer.h"

void showError(const char *msg){
    perror(msg);
    exit(1);
}