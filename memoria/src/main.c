#include "../include/main.h"

int main(int argc, char* argv[]) {
    t_config *config;
    
    config = levantar_config(getcwd(NULL, 0),"/memoria");
    saludar("memoria");
    return 0;
}
