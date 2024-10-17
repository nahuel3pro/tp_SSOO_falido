#include "../include/tools.h"

char *get_next_line(const char *filename)
{
    static FILE *file = NULL;
    static char buffer[1024];

    // If the file is not open yet, open it
    if (file == NULL)
    {
        file = fopen(filename, "r");
        if (file == NULL)
        {
            perror("Failed to open file");
            return NULL; // Return NULL if the file cannot be opened
        }
    }

    // Read the next line from the file
    if (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        return buffer; // Return the buffer containing the line
    }
    else
    {
        fclose(file); // Close the file when we reach the end
        file = NULL;  // Reset file pointer to allow re-opening the file later if needed
        return NULL;  // Return NULL to indicate EOF
    }
}

void initiate_registers(t_register *my_register)
{
    my_register->AX = 0;
    my_register->BX = 0;
    my_register->CX = 0;
    my_register->DX = 0;
    my_register->EX = 0;
    my_register->FX = 0;
    my_register->GX = 0;
    my_register->HX = 0;
}

bool thread_is_waiting(int pid, int tid)
{

    return false;
}

bool _has_equal_tid(int tid, int tid_queue)
{
    return (tid == tid_queue);
}

t_PCB get_process(int PID)
{
    bool process_contains(void *ptr)
    {
        t_PCB aux_pcb = (t_PCB)ptr;
        return aux_pcb->PID == PID;
    }
    // usar mutex acá
    return list_find(process_list, process_contains);
}

t_TCB get_thread(int PID, int TID)
{
    bool process_contains_TID(void *ptr)
    {
        t_TCB aux_tcb = (t_TCB)ptr;
        return aux_tcb->TID == TID;
    }

    t_PCB aux_pcb = get_process(PID);

    return list_find(aux_pcb->TIDs, process_contains_TID);
}