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