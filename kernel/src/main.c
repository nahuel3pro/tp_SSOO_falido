#include <../include/hello.h>
#include <../include/server.h>

int main(int argc, char* argv[]) {
    levantar_config(getcwd(NULL, 0),"/kernel");
    
    saludar("kernel");
    return 0;
}
