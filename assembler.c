#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/******************************************************
 * Structure Declaration
 *******************************************************/

struct Inst {
    char* name;
    char* op;
    char type;
    char* funct;
};

struct Data {
    int value;
    struct Data* next;
};

struct Text {
    int idx;
    char* d;
    char* s;
    char* t;
    unsigned int address;
    struct Text* next;
};

struct Sym {
    char* name;
    int size;
    unsigned int address;
    struct Data* first;
    struct Sym* next;
    struct Data* last;
};


/******************************************************
 * Global Variable Declaration
 *******************************************************/

struct Inst inst[20] = {
        {"addiu",   "001001", 'I', ""},
        {"addu",    "000000", 'R', "100001"},
        {"and",     "000000", 'R', "100100"},
        {"andi",    "001100", 'I', ""},
        {"beq",     "000100", 'I', ""},
        {"bne",     "000101", 'I', ""},
        {"j",       "000010", 'J', ""},
        {"jal",     "000011", 'J', ""},
        {"jr",      "000000", 'R', "001000"},
        {"lui",     "001111", 'I', ""},
        {"lw",      "100011", 'I', ""},
        {"nor",     "000000", 'R', "100111"},
        {"or",      "000000", 'R', "100101"},
        {"ori",     "001101", 'I', ""},
        {"sltiu",   "001011", 'I', ""},
        {"sltu",    "000000", 'R', "101011"},
        {"sll",     "000000", 'S', "000000"},
        {"srl",     "000000", 'S', "000010"},
        {"sw",      "101011", 'I', ""},
        {"subu",    "000000", 'R', "100011"}
};
struct Sym* Symbols;
struct Text* Texts;
struct Sym* finalSym = NULL;
struct Text* finalText = NULL;

int datasize, textsize;


/******************************************************
 * Function Declaration
 *******************************************************/

/* You may need the following functions */
char* change_file_ext(char* str);
char* num_to_bits(unsigned int num, int len);
unsigned int find_symbol(char* name);

/* You may fill the following functions */
void read_asm();
void subst_asm_to_num();
void print_bits();


/******************************************************
 * Function: main
 *
 * Parameters:
 *  int
 *      argc: the number of argument
 *  char*
 *      argv[]: array of a sting argument
 *
 * Return:
 *  return success exit value
 *
 * Info:
 *  The typical main function in C language.
 *  It reads system arguments from terminal (or commands)
 *  and parse an assembly file(*.s).
 *  Then, it converts a certain instruction into
 *  object code which is basically binary code.
 *
 *******************************************************/
int main(int argc, char* argv[]) {

    FILE* input, * output;
    char* filename;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <*.s>\n", argv[0]);
        fprintf(stderr, "Example: %s sample_input/example?.s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // reading the input file
    input = freopen(argv[1], "r", stdin);
    if (input == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    // creating the output file (*.o)
    filename = strdup(argv[1]); // strdup() is not a standard C library but fairy used a lot.
    if (change_file_ext(filename) == NULL) {
        fprintf(stderr, "'%s' file is not an assembly file.\n", filename);
        exit(EXIT_FAILURE);
    }

    output = freopen(filename, "w", stdout);
    if (output == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    Symbols = NULL;
    Texts = NULL;

    /******************************************************
     *  Let's complete the below functions!
     *
     *  read_asm(): Read the assmebly code.
     *  subst_asm_to_num(): Convert the assembly code to num.
     *  print_bits(): Print the results of assemble.
     *
     *******************************************************/
    read_asm();
    /*struct Sym* sym;
    for (sym = Symbols; sym != NULL; sym = sym->next) {
       printf(" sym %s ", sym->name);
    }*/
    print_bits();

    // freeing the memory
    free(filename);
    free(Symbols);
    free(Texts);

    fclose(input);
    fclose(output);
    exit(EXIT_SUCCESS);
}

/******************************************************
 * Function: change_file_ext
 *
 * Parameters:
 *  char
 *      *str: a raw filename (without path)
 *
 * Return:
 *  return NULL if a file is not an assembly file.
 *  return empty string
 *
 * Info:
 *  This function reads filename and converts it into
 *  object extention name, *.o
 *
 *******************************************************/
char* change_file_ext(char* str) {
    char* dot = strrchr(str, '.');

    if (!dot || dot == str || (strcmp(dot, ".s") != 0))
        return NULL;

    str[strlen(str) - 1] = 'o';
    return "";
}

/******************************************************
 * Function: num_to_bits
 *
 * Parameters:
 *  unsigned int
 *      num: a decimal number to be converted to binary bits
 *  int
 *      len: a length of bits
 *
 * Return:
 *  return string of binary bits
 *
 * Info:
 *  This function reads an integer number and converts it into
 *  binary bits represented by string.
 *
 *******************************************************/
char* num_to_bits(unsigned int num, int len) {
    char* bits = (char*)malloc(len + 1);
    int idx = len - 1, i;

    while (num > 0 && idx >= 0) {
        if (num % 2 == 1) {
            bits[idx--] = '1';
        }
        else {
            bits[idx--] = '0';
        }
        num /= 2;
    }

    for (i = idx; i >= 0; i--) {
        bits[i] = '0';
    }

    return bits;
}


/******************************************************
 * Function: read_asm
 *
 * Info:
 *  This function reads assembly code and saves the
 *  information into struct Sym, Data, Text, respectively.
 *
 *******************************************************/
void read_asm() {
    int tmp, i;
    unsigned int address;
    char temp[0x1000] = { 0 };
    struct Sym* temp_sym = malloc(sizeof(struct Sym));
    struct Data* temp_data = malloc(sizeof(struct Data));
    struct Text* temp_text = malloc(sizeof(struct Text));


    int start = 0;

    //Read .data region
    address = 0x10000000;
    for (i = 0; scanf("%s", temp) == 1;) {
        if (strcmp(temp, ".text") == 0) {
            /* blank */
            if (start != 0) {
                if (Symbols == NULL) {
                    Symbols = temp_sym;
                    finalSym = temp_sym;
                }
                else {
                    finalSym->next = temp_sym;
                    finalSym = temp_sym;
                }
            }
            break;
        }
        else if (temp[strlen(temp) - 1] == ':') {
            /* blank */

            if (start != 0) {
                if (Symbols == NULL) {
                    Symbols = temp_sym;
                    finalSym = temp_sym;
                }
                else {
                    finalSym->next = temp_sym;
                    finalSym = temp_sym;
                }
            }
            start = 1;

            struct Sym* newSym = malloc(sizeof(struct Sym));
            newSym->name = malloc(sizeof(10));

            strcpy(newSym->name, strtok(temp, ":"));
            newSym->address = address;
            address += 4;
            newSym->size = 4;

            scanf("%s", temp);
            scanf("%s", temp);

            int cvtData;
            if (strchr(temp, 'x') == NULL) {
                cvtData = atoi(temp);
            }
            else {
                cvtData = strtol(temp, NULL, 0);
            }

            struct Data* newData = malloc(sizeof(struct Data));
            newData->value = cvtData;

            newSym->first = newData;
            newSym->last = newData;

            temp_sym = newSym;
            newSym = NULL;


        }
        else if (strcmp(temp, ".word") == 0) {
            /* blank */

            scanf("%s", temp);
            int cvtData;
            if (strchr(temp, 'x') == NULL) {
                cvtData = atoi(temp);
            }
            else {
                cvtData = strtol(temp, NULL, 0);
            }
            struct Data* newData = malloc(sizeof(struct Data));
            newData->value = cvtData;
            temp_sym->last->next = newData;
            temp_sym->last = newData;
            temp_sym->size += 4;
            address += 4;

        }
    }

    datasize = address - 0x10000000;

    //Read .text region
    start = 0;
    address = 0x400000;
    for (i = 0; scanf("%s", temp) == 1;) {
        if (temp[strlen(temp) - 1] == ':') {
            /* blank */
            struct Sym* newSym = malloc(sizeof(struct Sym));
            newSym->name = malloc(sizeof(10));
            strcpy(newSym->name, strtok(temp, ":"));
            newSym->address = address;

            if(Symbols == NULL){
                Symbols = newSym;
                finalSym = newSym;
            } else{
                finalSym->next = newSym;
                finalSym = newSym;
            }

        }
        else {
            /* blank */
            struct Text* newText = malloc(sizeof(struct Text));
            newText->s = malloc(sizeof(10));
            newText->t = malloc(sizeof(10));
            newText->d = malloc(sizeof(10));

            newText->address = address;
            address += 4;

            if (strcmp(temp, "la") == 0) {
                scanf("%s", temp);
                strcpy(newText->s, temp);
                scanf("%s", temp);

                unsigned int num = find_symbol(temp);


                if (num % 0x00010000 == 0x00000000) {
                    newText->idx = 9;
                    newText->address = num / 0x00010000;

                    if (Texts == NULL) {
                        Texts = newText;
                        finalText = newText;
                    }
                    else {
                        finalText->next = newText;
                        finalText = newText;
                    }
                }
                else {
                    //lui
                    newText->idx = 9;
                    newText->address = num / 0x00010000;
                    if (Texts == NULL) {
                        Texts = newText;
                        finalText = newText;
                    }
                    else {
                        finalText->next = newText;
                        finalText = newText;
                    }
                    //ori
                    struct Text* newText2 = malloc(sizeof(struct Text));
                    newText2->s = malloc(sizeof(10));
                    newText2->t = malloc(sizeof(10));
                    newText2->d = malloc(sizeof(10));

                    newText2->idx = 13;
                    strcpy(newText2->s, newText->s);
                    strcpy(newText2->t, newText->s);
                    newText2->address = num % 0x00010000;
                    address += 4;
                    if (Texts == NULL) {
                        Texts = newText2;
                        finalText = newText2;
                    }
                    else {
                        finalText->next = newText2;
                        finalText = newText2;
                    }
                }

            }
            else {
                //printf("%s", temp);
                for (int i = 0; i < 20; i++) {
                    if (strcmp(temp, inst[i].name) == 0) newText->idx = i;
                }
                if (inst[newText->idx].type == 'R') {
                    if (strcmp(inst[newText->idx].name, "jr") == 0) { ///
                        scanf("%s", temp);
                        strcpy(newText->s, temp);
                    }
                    else {
                        scanf("%s", temp);
                        strcpy(newText->s, temp);
                        scanf("%s", temp);
                        strcpy(newText->t, temp);
                        scanf("%s", temp);
                        strcpy(newText->d, temp);
                    }
                }
                else if (inst[newText->idx].type == 'I') {

                    if (strcmp(inst[newText->idx].name, "bne") == 0 || strcmp(inst[newText->idx].name, "beq") == 0) {
                        scanf("%s", temp);
                        strcpy(newText->s, temp);
                        scanf("%s", temp);
                        strcpy(newText->t, temp);
                        scanf("%s", temp);
                        strcpy(newText->d, temp);
                        newText->address = address;
                    }
                    else if (strcmp(inst[newText->idx].name, "lw") == 0 || strcmp(inst[newText->idx].name, "sw") == 0) {
                        scanf("%s", temp);
                        strcpy(newText->t, temp);
                        scanf("%s", temp);
                        newText->address = strtol(strtok(temp, "()"), NULL, 0);
                        strcpy(newText->s, strtok(NULL, "()"));

                    }
                    else if (strcmp(inst[newText->idx].name, "lui") == 0) {
                        scanf("%s", temp);
                        strcpy(newText->s, temp);
                        scanf("%s", temp);
                        if (strchr(temp, 'x') == NULL) {
                            newText->address = atoi(temp);
                        }
                        else {
                            newText->address = strtol(temp, NULL, 0);
                        }

                    }
                    else {
                        scanf("%s", temp);
                        strcpy(newText->s, temp);
                        scanf("%s", temp);
                        strcpy(newText->t, temp);
                        scanf("%s", temp);
                        if (strchr(temp, 'x') == NULL) {
                            newText->address = atoi(temp);
                        }
                        else {
                            newText->address = strtol(temp, NULL, 0);
                        }
                    }

                }
                else if (inst[newText->idx].type == 'J') { // 확인 해야함.
                    scanf("%s", temp);
                    strcpy(newText->d, temp);

                    //newText->address = strtol(temp, NULL, 0);
                }
                else if (inst[newText->idx].type == 'S') {
                    scanf("%s", temp);
                    strcpy(newText->t, temp);
                    scanf("%s", temp);
                    strcpy(newText->d, temp);
                    scanf("%s", temp);
                    strcpy(newText->s, temp); //shmants
                }

                if (Texts == NULL) {
                    Texts = newText;
                    finalText = newText;
                }
                else {
                    finalText->next = newText;
                    finalText = newText;
                }
            }



        }
    }

    textsize = address - 0x400000;
}


/******************************************************
 * Function: subst_asm_to_num
 *
 * Info:
 *  This function converts assembly code to numbers.
 *
 *******************************************************/
void subst_asm_to_num() {
    struct Text* text;

    for (text = Texts; text != NULL; text = text->next) {
        /* blank */

    }
}

/******************************************************
 * Function: find_address
 *******************************************************/

unsigned int find_symbol(char* name) {
    for (struct Sym* temp = Symbols;temp!=NULL; temp = temp->next) {
        //printf("FFFF");
        if (temp->name != NULL)
        {
            if (strcmp(name, temp->name) == 0)
            {
                //printf("%s %x ", temp->name, temp->address);
                return temp->address;
            }
        }
        else
        {
            printf(" %x no ", temp->address);
        }
    }
    return 0;
}

/******************************************************
 * Function: print_bits
 *
 * Info:
 *  This function prints binary bit string converted by
 *  assembler. It will make binary object file of assembly code.
 *
 *******************************************************/
void print_bits() {

    struct Text* text;
    struct Sym* sym;
    struct Data* data;

    // print the header
    printf("%s", num_to_bits(textsize, 32));
    printf("%s", num_to_bits(datasize, 32));

    // print the body
    for (text = Texts; text != NULL; text = text->next) {
        //printf(" %s ", inst[text->idx].name);
        printf("%s", inst[text->idx].op);

        ///

        if (inst[text->idx].type == 'R') {
            /* blank */
            char tmp[100] = "";

            if (strcmp(inst[text->idx].name, "jr") == 0) {
                strncpy(tmp, text->s + 1, sizeof(text->s) - 1);
                printf("%s", num_to_bits(atoi(tmp), 5));
                printf("00000");
                printf("00000");
                printf("00000");
                printf("%s", inst[text->idx].funct);
            }
            else {
                strncpy(tmp, text->t + 1, sizeof(text->t) - 1);
                printf("%s", num_to_bits(atoi(tmp), 5));
                strncpy(tmp, text->d + 1, sizeof(text->d) - 1);
                printf("%s", num_to_bits(atoi(tmp), 5));
                strncpy(tmp, text->s + 1, sizeof(text->s) - 1);
                printf("%s", num_to_bits(atoi(tmp), 5));
                printf("00000");
                printf("%s", inst[text->idx].funct);
            }

        }
        else if (inst[text->idx].type == 'I') {
            /* blank */
            char tmp[100] = "";

            if (strcmp(inst[text->idx].name, "lw") == 0 || strcmp(inst[text->idx].name, "sw") == 0) {

                strncpy(tmp, text->s + 1, sizeof(text->s) - 1);
                printf("%s", num_to_bits(atoi(tmp), 5));
                strncpy(tmp, text->t + 1, sizeof(text->t) - 1);
                printf("%s", num_to_bits(atoi(tmp), 5));
                printf("%s", num_to_bits(text->address, 16));

            }
            else if (strcmp(inst[text->idx].name, "bne") == 0 || strcmp(inst[text->idx].name, "beq") == 0) {
                strncpy(tmp, text->s + 1, sizeof(text->s) - 1);
                printf("%s", num_to_bits(atoi(tmp), 5));
                strncpy(tmp, text->t + 1, sizeof(text->t) - 1);
                printf("%s", num_to_bits(atoi(tmp), 5));
                printf("%s", num_to_bits((find_symbol(text->d) - text->address) / 4, 16));

            }
            else if (strcmp(inst[text->idx].name, "lui") == 0) {
                printf("00000");
                strncpy(tmp, text->s + 1, sizeof(text->s) - 1);
                printf("%s", num_to_bits(atoi(tmp), 5));
                printf("%s", num_to_bits(text->address, 16));
            }
            else {
                strncpy(tmp, (text->t) + 1, sizeof(text->t) - 1);
                printf("%s", num_to_bits(atoi(tmp), 5));
                strncpy(tmp, text->s + 1, sizeof(text->s) - 1);
                printf("%s", num_to_bits(atoi(tmp), 5));
                printf("%s", num_to_bits(text->address, 16));
            }

        }
        else if (inst[text->idx].type == 'S') {
            /* blank */
            char tmp[100] = "";
            printf("00000");
            strncpy(tmp, text->d + 1, sizeof(text->d) - 1);
            printf("%s", num_to_bits(atoi(tmp), 5));
            strncpy(tmp, text->t + 1, sizeof(text->t) - 1);
            printf("%s", num_to_bits(atoi(tmp), 5));
            printf("%s", num_to_bits(atoi(text->s), 5));
            printf("%s", inst[text->idx].funct);
        }
        else { // J format
            /* blank */
            printf("%s", num_to_bits(find_symbol(text->d)/4, 26));
        }
    }

    for (sym = Symbols; sym != NULL; sym = sym->next) {
        /* blank */
        for (int i = 0; i < (sym->size / 4); sym->first = sym->first->next) {
            printf("%s", num_to_bits(sym->first->value, 32));
            i++;
        }

    }
    printf("\n"); // To exclude "No newline at end of file"
}