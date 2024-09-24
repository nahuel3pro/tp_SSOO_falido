#include "../include/tools.h"

// void asignar_registro(char *registro_in, char valor, t_Registro_Cpu *registros){

//     if(!strcmp(registro_in,"AX"))
//         registros->AX = valor;
    
//     if(!strcmp(registro_in,"BX"))
//         registros->BX = valor;

//     if(!strcmp(registro_in,"CX"))
//         registros->CX = valor;
    
//     if(!strcmp(registro_in,"DX"))
//         registros->DX = valor;

//     if(!strcmp(registro_in,"PC"))
//         registros->PC = valor;
    
//     if(!strcmp(registro_in,"DI"))
//         registros->DI = valor;

//     if(!strcmp(registro_in,"SI"))
//         registros->SI = valor;
    
//     if(!strcmp(registro_in,"EAX"))
//         registros->EAX = valor;

//     if(!strcmp(registro_in,"EBX"))
//         registros->EBX = valor;

//     if(!strcmp(registro_in,"ECX"))
//         registros->ECX = valor;

//     if(!strcmp(registro_in,"EDX"))
//         registros->EDX = valor;

// }

void set_registro(t_register* registros, char *registro_in, int valor)
{
    if(!strcmp(registro_in,"PC"))
        registros->PC = valor;

    if(!strcmp(registro_in,"AX"))
        registros->AX = valor;
    
    if(!strcmp(registro_in,"BX"))
        registros->BX = valor;

    if(!strcmp(registro_in,"CX"))
        registros->CX = valor;
    
    if(!strcmp(registro_in,"DX"))
        registros->DX = valor;

    if(!strcmp(registro_in,"EX"))
        registros->EX = valor;

    if(!strcmp(registro_in,"FX"))
        registros->FX = valor;

    if(!strcmp(registro_in,"GX"))
        registros->GX = valor;

    if(!strcmp(registro_in,"HX"))
        registros->HX = valor;

}

char obtener_registro(char *registro_in, t_register *registros)
{
    uint32_t valor =0;
    if(!strcmp("PC", registro_in))
        valor = registros->PC;
    if(!strcmp("AX", registro_in))
        valor = registros->AX;
    if(!strcmp("BX", registro_in))
        valor = registros->BX;
    if(!strcmp("CX", registro_in))
        valor = registros->CX;
    if(!strcmp("DX", registro_in))
        valor = registros->DX;
    if(!strcmp("EX", registro_in))
        valor = registros->EX;
    if(!strcmp("FX", registro_in))
        valor = registros->FX;
    if(!strcmp("GX", registro_in))
        valor = registros->GX;
    if(!strcmp("HX", registro_in))
        valor = registros->HX;


    return valor;
}