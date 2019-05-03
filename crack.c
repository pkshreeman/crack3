#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "md5.h"

const int PASS_LEN=50;        // Maximum any password can be
const int HASH_LEN=33;        // Length of MD5 hash strings

char *contents;
struct entry * cc;
char *pointerToContents;

// Stucture to hold both a plaintext password and a hash.
struct entry 
{
    char *password;
    char *hash;
};

// This was not on TODO list, but was in cartoon, and I suspect necessary here
int file_length(char *filename)
{
    struct stat info;
    int res = stat(filename, &info);
    if (res == -1) return -1;
    else return info.st_size;
}

// Implementing the comparing function for qsort
int comp(const void *a, const void *b)
{
    struct entry *ca = (struct entry *)a;
    struct entry *cb = (struct entry *)b;
    return strcmp(ca->hash, cb->hash);
}

// Bsearch compare function based on cartoon.c
int cbcomp(const void *target, const void *elem)
{
    char *target_str = (char *)target;
    struct entry *celem = (struct entry *)elem;
    return strcmp(target_str, (*celem).hash);
}

// Read Dictionary Function - read the rockyou.txt, then create hashes
struct entry *read_dictionary(char *filename, int *size)
{
    int filelength = file_length(filename);
    printf("The filelength is %d\n", filelength);
    FILE *c = fopen(filename, "r");
    if (!c){
        printf("Can't open %s\n", filename);
    }
    
    char *contents = malloc(filelength);
    fread(contents, 1, filelength, c);
    fclose(c);
    
    // Replacing \n with \0
    int lines = 0;
    for (int i = 0; i < filelength; i++){
            if(contents[i] == '\n'){
                contents[i] = '\0';
                lines++;
            }
    }
    
    //printf("lines %d\n", lines);

    //First array setup
    // printf("Did we get to this point?\n");
    cc = malloc(lines * sizeof(struct entry));
    cc[0].password = &contents[0];
    pointerToContents = &contents[0];
    cc[0].hash = md5(&contents[0], strlen(&contents[0]));
    
    //Test
    //printf("The first attempt of cc assignment: %s  %s\n", cc[0].password, cc[0].hash);
    
    int count = 1;
    for (int i = 0; i < filelength-1; i++){
        if (contents[i] == '\0'){
            cc[count].password = &contents[i+1];
            cc[count].hash = md5(&contents[i+1], strlen(&contents[i+1]));
            count++;
        }
    }
    
    /*Test output -- de/comment accordingly
    for (int i = 0; i < lines; i++){
        printf("%d %s %s \n", i, cc[i].password, cc[i].hash);
    }
    */
    
    *size = lines;
    return cc;
}


int main(int argc, char *argv[])
{
    if (argc < 3) 
    {
        printf("Usage: %s hash_file dict_file\n", argv[0]);
        exit(1);
    }

    // TODO: Read the dictionary file into an array of entry structures
    int dlen;
    printf("Creating dictionary...\n");
    struct entry *dict = read_dictionary(argv[2], &dlen);

    //printf("Dictionary at first index: %s %s\n", dict[0].password, dict[0].hash);
    printf("Sorting the dictionary by hashes...\n");
    qsort(dict, dlen, sizeof(struct entry), comp);
    
    /*Test output -- de/comment accordingly
    for (int i = 0; i < dlen; i++){
        printf("%d %s %s \n", i, dict[i].password, dict[i].hash);
    }
    */

    // Open the hash file for reading.
    FILE *h = fopen (argv[1],"r");
    if (!h){
        printf("Can't open hash file");
        exit(1);
    }
    
    char hashlines[50];
    printf("Cracked? %-15s  --> %s\n","Password","Hash");
    printf("=============================================================\n");
    while(fgets(hashlines,50,h) != NULL){
        hashlines[strlen(hashlines) -1] = '\0';
        for (int i = 0; i < dlen; i++){
            
            struct entry *found = bsearch(hashlines, dict, dlen, 
            sizeof(struct entry), cbcomp);
            if (found == NULL){
                printf("Not found/n");
            }
            else{
                printf("Cracked! %-15s  --> %s\n", found->password, found->hash);
                break;
            }
        }
    }
 
    fclose(h);
    for (int i = dlen -1; i > -1; i--){
        free(dict[i].hash);
    }
   
    free(dict);
    free(pointerToContents);
}
