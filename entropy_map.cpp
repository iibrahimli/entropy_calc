#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unordered_map>

// error codes
#define ERR_BAD_ARGUMENTS      1
#define ERR_OPENING_FILE       2
#define ERR_BIG_ALPHABET_SIZE  3
#define ERR_BIG_SYMBOL_COUNT   4

typedef unsigned char byte;

void print_usage(char **argv);
unsigned long symbol_to_index(const byte symbol[], const unsigned int n);

int main(int argc, char* argv[]){
    
    unsigned int n = 1;                  // the n in n-graph
    const char* filename;

    // both n and filename given (ex: ./entropy -a 2 file.txt)
    if(argc == 4){
        if(!strcmp(argv[1], "-a")){
            n = atoi(argv[2]);
            filename = argv[3];
        }
        else{
            print_usage(argv);
            return ERR_BAD_ARGUMENTS;
        }
    }
    // only filename given (ex: ./entropy file.txt)
    else if(argc == 2){
        filename = argv[1];
        printf("Alphabet size not given, assuming default value 1\n");
    }
    // smth wrong
    else{
        print_usage(argv);
        return ERR_BAD_ARGUMENTS;
    }

    FILE* fp = fopen(filename, "rb");
    if(fp == NULL){
        fprintf(stderr, "Could not open %s\n", filename);
        return ERR_OPENING_FILE;
    }

    unsigned long n_alpha = (unsigned long) 0x1<<(n<<3);  // # of possible symbols

    std::unordered_map<unsigned long, unsigned long> counts;  // # of occurences of each symbol
    unsigned long  filesize = 0;         // size of file in bytes
    double         entropy  = 0;

    // warning if alphabet size is greater than index capacity
    if(sizeof symbol_to_index(NULL, 0) < n){
        fprintf(stderr, "Alphabet size (%u) is greater than index size (%lu). Aborting\n", n, sizeof symbol_to_index(NULL, 0));
        return ERR_BIG_ALPHABET_SIZE;
    }

    // warning if alphabet size is greater than n_alpha capacity
    if(sizeof n_alpha < n){
        fprintf(stderr, "Alphabet size in bytes (%u) is greater than size of n_alpha (%lu). Aborting\n", n<<3, sizeof n_alpha);
        return ERR_BIG_ALPHABET_SIZE;
    }


    for(int i=0; i<n_alpha; i++){
        counts[i] = 0;
    }

    // getting size of file
    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    byte* buffer = (byte*) malloc(sizeof *buffer * filesize);

    // if malloc fails
    if(buffer == NULL){
        fprintf(stderr, "Could not load whole file into memory, proceeding symbol by symbol, be patient\n");

        unsigned long buff_size = 1;
        buffer = (byte*) malloc(sizeof *buffer * n * buff_size);

        // reading file symbol by symbol, incrementing each value
        while(fread(buffer, sizeof *buffer, buff_size, fp) == buff_size){
            for(int i=0; i<buff_size; i+=n){
                counts[symbol_to_index(&buffer[i], n)]++;
            }
        }
    }
    // allocation OK
    else{
        // reading whole file
        fread(buffer, sizeof *buffer, filesize, fp);

        for(int i=0; i<filesize; i+=n){
            counts[symbol_to_index(&buffer[i], n)]++;
        }
    }

    fclose(fp);

    // calculating probabilities and entropy
    for(int i=0; i<n_alpha; i++){
        double prob = ((double) counts[i]) / (filesize * n);
        if(prob != 0)
            entropy -= prob * log2(prob);
    }

    free(buffer);

    printf("Entropy of %s : %.5lf\n", filename, entropy);

    return 0;
}


void print_usage(char **argv){
    printf("Usage: %s [-a N] filename\nN : size of the alphabet\n", argv[0]);
}

unsigned long symbol_to_index(const byte symbol[], const unsigned int n){
    unsigned long idx = 0;

    for(int i=n-1; i>=0; i--){
        idx <<= 8;
        idx += symbol[i];
    }

    return idx;
}