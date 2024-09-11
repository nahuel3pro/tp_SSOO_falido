#include <../include/operaciones_kernel.h>

t_list *recibir_instrucciones(char* file_name) {
    // Nombre del archivo
    const char *dir_actual = getcwd(NULL,0);
    t_list *lista = list_create();

    if(dir_actual == NULL){
        perror("Error al obtener el directorio actual");
        return NULL;
    }

    char filename[MAX_NOMBRE];
    snprintf(filename, sizeof(filename), "%s/test_psdc/%s", dir_actual, file_name);

    free(dir_actual);

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    char **lines = malloc(MAX_NOMBRE * sizeof(char *));
    if (lines == NULL) {
        perror("Error al asignar memoria para las líneas");
        fclose(file);
        return NULL;
    }

    char buffer[MAX_NOMBRE];
    int line_count = 0;

    // Leer el archivo línea por línea
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Asignar memoria para la línea y copiar el contenido del buffer
        lines[line_count] = malloc(strlen(buffer) + 1);
        if (lines[line_count] == NULL) {
            perror("Error al asignar memoria para una línea");
            // Liberar las líneas ya asignadas en caso de error
            for (int i = 0; i < line_count; i++) {
                free(lines[i]);
            }
            free(lines);
            fclose(file);
            return NULL;
        }
        strcpy(lines[line_count], buffer);
        line_count++;
    }

    // Cerrar el archivo
    fclose(file);

    if (line_count < MAX_NOMBRE) {
        lines[line_count] = NULL;  // Añade un NULL al final para marcar el fin
    }

    for(int i = 0; lines[i] != NULL && i<MAX_OPERACIONES_LENGTH;i++){
        list_add(lista,lines[i]);
    }

    return lista;

}

// Función para liberar la memoria asignada por recibir_instrucciones
void liberar_instrucciones(t_list *lista) {
    // Verificar si la lista está vacía o es NULL
    if (lista == NULL || list_is_empty(lista)) {
        return;
    }
    
    // Limpiar y destruir los elementos de la lista utilizando free
    list_clean_and_destroy_elements(lista, (void*)free);
}
