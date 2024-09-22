#include "../include/tools.h"

char *get_next_line(const char *filename, int* last_line_read)
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
        *last_line_read = ftell(file);
        return buffer; // Return the buffer containing the line
    }
    else
    {
        fclose(file); // Close the file when we reach the end
        file =
            NULL;    // Reset file pointer to allow re-opening the file later if needed
        return NULL; // Return NULL to indicate EOF
    }
}