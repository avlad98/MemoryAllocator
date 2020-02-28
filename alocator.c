#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_INDEX 50000

unsigned char *arena = NULL;
uint32_t index_block = 0;
uint32_t arena_size = 0;
uint32_t numar_blocuri = 0;


void INITIALIZE(uint32_t size){

    arena = malloc(size * sizeof(unsigned char));
    index_block = 0;
    for (int i = 0; i < size; i++)
    {
        *(arena+i) = 0;
    }
}

void FINALIZE(){
    free(arena);
    index_block = 0;
    arena_size = 0;
    numar_blocuri = 0;
}

void DUMP(uint32_t size){
    unsigned int indice_curent = 0, i, j;

    if(size != 0){
        printf("%08X\t", indice_curent);
        for (i = 1, j = 0; j < size; i++, j++)
        {
            if(i % 9 == 0){
                printf(" ");
            }

            if(i % 16 == 0){
                printf("%02X ", *(arena+j));
                if(j+1 < size){
                    indice_curent++;
                    printf("\n%07X0\t", indice_curent);
                    i = 0;
                }
            }else
                printf("%02X ", *(arena+j));
        }
        printf("\n");
    }
}

void ALLOC(uint32_t size){

    if( (size + 11) < arena_size){
        
        //Initializare Bloc 1
        if(numar_blocuri == 1){
        *((int*)(arena + index_block)) = 0;
        *((int*)(arena + index_block + 4)) = 0;
        *((int*)(arena + index_block + 8)) = size;  
        printf("%d\n", index_block + 12); 
        }

        int last_block = 0, next_block = 0, found = 0;
        if(numar_blocuri > 1){
                //Se cauta loc liber intre primul bloc si inceputul arenei
            if( index_block >= (size + 12) ){
                found = 1;
                *((int*)(arena)) = index_block;
                *((int*)(arena + 4)) = 0;
                *((int*)(arena + 8)) = size;  
                printf("%d\n", index_block + 12);
                index_block = 0;
            }else 
            {   
                //Se cauta loc intre blocuri
                int previous_block, next_block, found_block, count_blocks;
                
                count_blocks = 2;
                previous_block = index_block;
                next_block = *((int*)(arena + index_block));

                while(count_blocks < numar_blocuri){ 
                    count_blocks++; 
                    if((next_block - (*((int*)(arena + previous_block + 8)) + previous_block + 12)) >= size){
                        //Initializare bloc in zona libera gasita
                        found_block = *((int*)(arena + previous_block + 8)) + previous_block + 12;
                        *((int*)(arena + found_block)) = next_block;
                        *((int*)(arena + found_block + 4)) = previous_block;
                        *((int*)(arena + found_block + 8)) = size;

                        //Modificare parametru bloc anterior
                        *((int*)(arena + previous_block)) = found_block;               

                        //Modificare parametru bloc urmator
                        *((int*)(arena + next_block)) = found_block;

                        found = 1;
                    }
                    else{
                        previous_block = next_block;
                        next_block = *((int*)(arena + next_block));
                    }
                }
                if (found)
                {
                    printf("%d\n", found_block + 12);
                }
            }

            if(found != 1){  
                //Se avanseaza la alte blocuri
                //Cauta ultimul bloc
                while( *((int*)(arena + index_block + last_block)) != 0){
                    last_block = *((int*)(arena + index_block + last_block)) + last_block;
                }

                next_block = *((int*)(arena + index_block + last_block + 8)) + 12;
                //Verifica daca e loc dupa ultimu bloc si initializeaza
                if((arena_size - (next_block + 12)) > size){
                    *((int*)(arena + next_block)) = 0;
                    *((int*)(arena + next_block + 4)) = *((int*)(arena + last_block));
                    *((int*)(arena + next_block + 8)) = size;
                    *((int*)(arena + last_block)) = next_block;
                    printf("%d\n", next_block+12);
                }
            }
        } 
    }   
}

void FREE(uint32_t index){
        numar_blocuri--;
        int next_block = *((int*)(arena + index));
        int previous_block = *((int*)(arena + index + 4));

        if (previous_block != 0)
        {
            *((int*)(arena + previous_block)) = next_block;
        }else{
            index_block = index;
        }

        if (next_block != 0)
        {
           *((int*)(arena + next_block + 4)) = previous_block;
        }

        int end_index = *((int*)(arena + index + 8)) + 12;
        for (int i = index; i < end_index; ++i)
        {
           *(arena+i) = 0;
        }

}

void parse_command(char* cmd)
{
    const char* delims = " \n";

    char* cmd_name = strtok(cmd, delims);
    if (!cmd_name) {
        goto invalid_command;
    }

    if (strcmp(cmd_name, "INITIALIZE") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        // TODO - INITIALIZE
        arena_size = size;
        INITIALIZE(size);

    } else if (strcmp(cmd_name, "FINALIZE") == 0) {
        // TODO - FINALIZE
        FINALIZE();

    } else if (strcmp(cmd_name, "DUMP") == 0) {
        // TODO - DUMP
        DUMP(arena_size);

    } else if (strcmp(cmd_name, "ALLOC") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        // TODO - ALLOC
        numar_blocuri++;
        ALLOC(size);

    } else if (strcmp(cmd_name, "FREE") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
        // TODO - FREE
        FREE(index-12);

    } else if (strcmp(cmd_name, "FILL") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        char* value_str = strtok(NULL, delims);
        if (!value_str) {
            goto invalid_command;
        }
        uint32_t value = atoi(value_str);
        // TODO - FILL

    } else if (strcmp(cmd_name, "ALLOCALIGNED") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        char* align_str = strtok(NULL, delims);
        if (!align_str) {
            goto invalid_command;
        }
        uint32_t align = atoi(align_str);
        // TODO - ALLOCALIGNED

    } else if (strcmp(cmd_name, "REALLOC") == 0) {
        printf("Found cmd REALLOC\n");
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        // TODO - REALLOC

    } else {
        goto invalid_command;
    }

    return;

invalid_command:
    printf("Invalid command: %s\n", cmd);
    exit(1);
}

int main(void)
{
    ssize_t read;
    char* line = NULL;
    size_t len;

    /* parse input line by line */
    while ((read = getline(&line, &len, stdin)) != -1) {
        /* print every command to stdout */
        printf("%s", line);

        parse_command(line);
    }

    free(line);

    return 0;
}
