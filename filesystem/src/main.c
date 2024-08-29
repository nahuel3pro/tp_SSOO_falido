#include <../include/hello.h>
#include <../include/server.h>

int main(int argc, char *argv[])
{
    levantar_config(getcwd(NULL, 0), "filesystem");

    saludar("filesystem");
    return 0;
}
