#include <../include/hello.h>
#include <../include/server.h>

int main(int argc, char* argv[]) {
    levantar_config(getcwd(NULL, 0),"cpu");

    saludar("cpu");
    return 0;
}
