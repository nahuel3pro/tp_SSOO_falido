#include "../include/tools.h"

char *get_next_line(const char *filename)
{
    FILE *file = NULL;
    char buffer[256];

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

void load_list_instructions(t_list *list_instructions, const char *path)
{
    const int MAX_LINE_LENGTH = 256;
    FILE *pseudocodigo;
    char buffer[MAX_LINE_LENGTH];

    pseudocodigo = fopen(path, "r");

    if (pseudocodigo == NULL)
    {
        log_error(log, "No se pudo abrir el archivo.\n");
        abort();
    }

    while (fgets(buffer, MAX_LINE_LENGTH, pseudocodigo) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = '\0';
        char *buffer_add = malloc(sizeof(buffer));
        strcpy(buffer_add, buffer);
        list_add(list_instructions, buffer_add);
    }

    fclose(pseudocodigo);
}

void initiate_registers(t_register *registro)
{
    registro->PC = 0;
    registro->AX = 0;
    registro->BX = 0;
    registro->CX = 0;
    registro->DX = 0;
    registro->EX = 0;
    registro->FX = 0;
    registro->GX = 0;
    registro->HX = 0;
    registro->base = 0;
    registro->limite = 0;
}

t_PCB process_initiate(int PID, int size)
{
    t_PCB pcb = malloc(sizeof(*pcb));
    pcb->PID = PID;
    pcb->size = size;
    pcb->TIDs = list_create();

    return pcb;
}

t_TCB thread_initiate(char *file_path, int thread_priority, int PID, int TID)
{
    t_TCB tcb = malloc(sizeof(*tcb));
    tcb->TID = TID;
    tcb->PID = PID;
    tcb->priority = thread_priority;
    tcb->instructions = list_create();
    initiate_registers(&tcb->registers);
    load_list_instructions(tcb->instructions, file_path);

    return tcb;
}

void update_registers(t_register *mem_reg, t_register new_registers)
{
    mem_reg->PC = new_registers.PC;
    mem_reg->AX = new_registers.AX;
    mem_reg->BX = new_registers.BX;
    mem_reg->CX = new_registers.CX;
    mem_reg->DX = new_registers.DX;
    mem_reg->EX = new_registers.EX;
    mem_reg->FX = new_registers.FX;
    mem_reg->GX = new_registers.GX;
    mem_reg->HX = new_registers.HX;
    mem_reg->base = new_registers.base;
    mem_reg->limite = new_registers.limite;
}

void safe_pcb_add(t_list *list, t_PCB pcb, pthread_mutex_t *mutex)
{
    pthread_mutex_lock(mutex);
    list_add(list, pcb);
    pthread_mutex_unlock(mutex);
}

t_PCB safe_pcb_remove(t_list *list, pthread_mutex_t *mutex)
{
    t_PCB pcb;
    pthread_mutex_lock(mutex);
    pcb = list_remove(list, 0);
    pthread_mutex_unlock(mutex);
    return pcb;
}

// experimento de encontrar PID o TID usando list_find

t_PCB get_process(int PID)
{
    bool process_contains(void *ptr)
    {
        t_PCB aux_pcb = (t_PCB)ptr;
        return aux_pcb->PID == PID;
    }
    // usar mutex acá
    pthread_mutex_lock(&mutex_process_list);
    t_TCB aux_tcb = list_find(process_list, process_contains);
    pthread_mutex_unlock(&mutex_process_list);
    return aux_tcb;
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