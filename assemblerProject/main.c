#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqoai.h>
#include "assemble.h"


void calculationBasedInTypes(int op, struct instruction *currentInstruction, struct symbolTable *symT, char hex_table[16],char lower[5], char *token2, int symbolTabLength, int PC);
int hex2int(char *hex);
void int2hex16(char *lower, int a);
void duplicatedLabel(struct symbolTable *symT,int symbolTabLength);
int validOpCode(char *token,char *instructions[]);
int lineNumber;
int main(int argc, char **argv){
    FILE *assp,*machp,*fopen();
    struct symbolTable *pSymTab;
    int symTabLen;
    int i,j,found,noInsts;
    struct instruction *currInst;
    size_t lineSize;
    char *line;
    char *token;
    char *instructions[]={"add","sub","slt","or","nand",
                          "addi","slti","ori","lui","lw","sw","beq","jalr",
                          "j","halt"};
    int instCnt=0;
    char hexTable[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    char lower[5];
    i=0;
    j=0;
    line=(char *)malloc(72);
    currInst=(struct instruction *)malloc(sizeof(struct instruction));
    if(argc < 3){
        printf("***** Please run this program as follows:\n");
        printf("***** %s assprog.as machprog.m\n",argv[0]);
        printf("***** where assprog.as is your assembly program\n");
        printf("***** and machprog.m will be your machine code.\n");
        exit(1);
    }
    if((assp=fopen(argv[1],"r")) == NULL){
        printf("%s cannot be opened\n",argv[1]);
        exit(1);
    }
    if((machp=fopen(argv[2],"w+")) == NULL){
        printf("%s cannot be opened\n",argv[2]);
        exit(1);
    }
    symTabLen = findSymTabLen(assp);
    pSymTab = (struct symbolTable* ) malloc(symTabLen * sizeof(struct symbolTable));
    for (int k = 0; k < symTabLen; ++k) {
        pSymTab[k].symbol = (char*) malloc(7);
    }
    noInsts = fillSymTab(pSymTab, assp);
//    for (int k = 0; k < symTabLen; ++k) {
//        printf("%s: %d \n", pSymTab[k].symbol, pSymTab[k].value);
//    }
    duplicatedLabel(pSymTab,symTabLen);
    char *newLine = (char *) malloc(72);
    int programC = -1;
    while (fgets(newLine, 72, assp) != NULL){
        programC++;
        if (newLine[0] == ' ' || newLine[0] == '\t'){       // if () then the first word will be the op code
            token = strtok(newLine, "\t,\n");
            int opCode = validOpCode(token, instructions);
            if (opCode != -1){
                calculationBasedInTypes(opCode, currInst, pSymTab, hexTable, lower, token, symTabLen, programC);
                printf("%d\n",currInst->intInst);
                fprintf(machp,"%d\n", currInst->intInst);
            }
            else{
                puts("ERROR! Invalid Opcode\n");
                exit(1);
            }
        }
        else{
            Boolean isInst = FALSE;
            token = strtok(newLine, "\t ");
            token = strtok(NULL, "\t\n ");
            for (int opCode = 0; opCode < 15; ++opCode) {
                if (strcmp(instructions[opCode], token) == 0){
                    isInst = TRUE;
                    calculationBasedInTypes(opCode, currInst, pSymTab, hexTable, lower, token, symTabLen, programC);
                    printf("%d\n",currInst->intInst);
                    fprintf(machp,"%d\n",currInst->intInst);
                }
            }
            if (isInst == FALSE){
                if(strcmp(token, ".fill") == 0){        //it can be a number or a label after a .fill  .fill: put a number in memory
                    token = strtok(NULL, "\t\n");
                    int foundLabel = 0;
                    int index = 0;
                    for (int k = 0; k < symTabLen; ++k) {
                        if (strcmp(token, pSymTab[i].symbol) == 0){
                            foundLabel = 1;
                            index = i;
                        }
                    }
                    if (foundLabel){
                        currInst->imm = pSymTab[index].value;       //put the address of the label in imm
                    }
                    else if (isalpha(token[0])){
                        puts("ERROR! Invalid Label\n");     //it is not a label. so it can not be a word. it has to be a number!
                        exit(1);
                    }
                    else{
                        currInst->imm = atoi(token);        //put the exact number in imm
                    }
                    currInst->intInst = currInst->imm;
                    printf("%d\n",currInst->intInst);
                    fprintf(machp,"%d\n",currInst->intInst);
                }
                else if (strcmp(token, ".space") == 0){     //.space: allocate memory using the number after .space
                    token = strtok(NULL, "\t\n");
                    int foundLabel = 0;
                    int index = 0;
                    for (int k = 0; k < symTabLen; ++k) {
                        if (strcmp(token, pSymTab[i].symbol) == 0){
                            foundLabel = 1;
                            index = i;
                        }
                    }
                    if (foundLabel){
                        currInst->imm = pSymTab[index].value;       //put the address of the label in imm
                    }
                    else if (isalpha(token[0])){
                        puts("ERROR! Invalid Label\n");     //it is not a label. so it can not be a word. it has to be a number!
                        exit(1);
                    }
                    else{
                        currInst->imm = atoi(token);        //put the exact number in imm
                    }
                     //(char *) malloc(currInst->imm);
                }
                else{
                    puts("ERROR! Invalid Opcode\n");
                    exit(1);
                }
            }
        }
    }
    return 0;
//    fclose(assp);
//    fclose(machp);
}
int findSymTabLen(FILE *inputFile){
    int count=0;
    size_t lineSize;
    char *line;
    line=(char *)malloc(72);
    while(fgets(line,lineSize,inputFile) != NULL){
        if((line[0] == ' ') || (line[0] == '\t'));
        else
            count++;
    }
    rewind(inputFile);
    free(line);
    return count;
}
int fillSymTab(struct symbolTable *symT,FILE *inputFile){
    int lineNo=0;
    int lineSize = 72;
    char *line;
    int i=0;
    char *token;
    line=(char *)malloc(72);
    while(fgets(line,lineSize,inputFile) != NULL){
        if((line[0] == ' ') || (line[0] == '\t'));
        else{
            token=strtok(line,"\t, ");
            strcpy(symT[i].symbol,token);
            symT[i].value=lineNo;
            i++;
        }
        lineNo++;
    }
    if (lineNo > 65536)
        printf("ERROR! Memory Overflow");
    else
        lineNumber = lineNo;
    rewind(inputFile);
    free(line);
    return lineNo;
}
void calculationBasedInTypes(int op, struct instruction *currentInstruction, struct symbolTable *symbolT, char hexTable[16],char lower[5], char *token2, int symbolTabLength, int programC) {
    if (op >= 0 && op < 5) {       //R type instruction
        currentInstruction->instType = RTYPE;
        currentInstruction->mnemonic = (char *) malloc(5);
        strcpy(currentInstruction->mnemonic, token2);
        token2 = strtok(NULL, ",\t\n ");
        currentInstruction->rd = atoi(token2);
        token2 = strtok(NULL, ",\t\n ");
        currentInstruction->rs = atoi(token2);
        token2 = strtok(NULL, ",\t\n ");
        currentInstruction->rt = atoi(token2);
        currentInstruction->PC = currentInstruction->PC + 1;
        currentInstruction->inst[0] = '0';
        currentInstruction->inst[1] = hexTable[op];
        currentInstruction->inst[2] = hexTable[currentInstruction->rs];
        currentInstruction->inst[3] = hexTable[currentInstruction->rt];
        currentInstruction->inst[4] = hexTable[currentInstruction->rd];
        currentInstruction->inst[5] = '0';
        currentInstruction->inst[6] = '0';
        currentInstruction->inst[7] = '0';
        currentInstruction->inst[8] = '\0';
        int result = hex2int(currentInstruction->inst);
        currentInstruction->intInst = result;
    } else if (op > 4 && op <= 12) {        //I type instruction
        currentInstruction->instType = ITYPE;
        currentInstruction->mnemonic = (char *) malloc(5);
        strcpy(currentInstruction->mnemonic, token2);
        if (strcmp(currentInstruction->mnemonic, "jalr") == 0) {
            token2 = strtok(NULL, ",\t\n ");
            currentInstruction->rt = atoi(token2);
            token2 = strtok(NULL, ",\t\n ");
            currentInstruction->rs = atoi(token2);
            currentInstruction->imm = 0;                //offset of jalr is 0!
        } else if (strcmp(currentInstruction->mnemonic, "lui") == 0) {
            token2 = strtok(NULL, ",\t\n ");
            currentInstruction->rt = atoi(token2);
            token2 = strtok(NULL, ",\t\n ");
            currentInstruction->imm = atoi(token2);
            currentInstruction->rs = 0;
        } else if (strcmp(currentInstruction->mnemonic, "beq") == 0) {
            token2 = strtok(NULL, ",\t\n ");
            currentInstruction->rs = atoi(token2);
            token2 = strtok(NULL, ",\t\n ");
            currentInstruction->rt = atoi(token2);
            token2 = strtok(NULL, ",\t\n ");
            int index = -1;
            for (int i = 0; i < symbolTabLength; i++) {
                if (strcmp(symbolT[i].symbol, token2) == 0) {
                    index = symbolT[i].value;
                }
            }
                if (index != -1) {          //if it is a label
                    currentInstruction->imm = index - (programC + 1);
                } else if (isalpha((token2[0]))) {
                    puts("ERROR! Invalid Label\n");
                    exit(1);
                } else {
                    long num = atol(token2);
                    if (num < lineNumber) {
                        num = num - (programC + 1);
                        currentInstruction->imm = num;
                    } else {
                        puts("ERROR! Invalid Offset\n");
                        exit(1);
                    }
                }
        } else {       //addi ori slti sw lw
            token2 = strtok(NULL, ",\t\n ");
            currentInstruction->rs = atoi(token2);
            token2 = strtok(NULL, ",\t\n ");
            currentInstruction->rt = atoi(token2);
            token2 = strtok(NULL, ",\t\n ");
            int index = -1;
            for (int i = 0; i < symbolTabLength; i++) {
                if (strcmp(symbolT[i].symbol, token2) == 0) {
                    index = i;
                }
            }
            if (index != -1) {
                currentInstruction->imm = symbolT[index].value;
            } else if (isalpha((token2[0]))) {
                puts("ERROR! Invalid Label\n");
                exit(1);
            } else {
                long num = atol(token2);
                if (num < 65536 && num > -65536) {
                    currentInstruction->imm = num;
                } else {
                    puts("ERROR! Invalid Offset\n");
                    exit(1);
                }
            }
        }
    currentInstruction->PC = currentInstruction->PC + 1;
    currentInstruction->inst[0] = '0';
    currentInstruction->inst[1] = hexTable[op];
    currentInstruction->inst[2] = hexTable[currentInstruction->rs];
    currentInstruction->inst[3] = hexTable[currentInstruction->rt];
    int2hex16(lower, currentInstruction->imm);
    currentInstruction->inst[4] = lower[0];
    currentInstruction->inst[5] = lower[1];
    currentInstruction->inst[6] = lower[2];
    currentInstruction->inst[7] = lower[3];
    currentInstruction->inst[8] = '\0';
    int result = hex2int(currentInstruction->inst);
    currentInstruction->intInst = result;

   }else if (op == 13 || op == 14) {        //J type instruction
        currentInstruction->instType = JTYPE;
        currentInstruction->mnemonic = (char *) malloc(5);
        strcpy(currentInstruction->mnemonic, token2);
        if (strcmp(currentInstruction->mnemonic, "halt") == 0) {
            currentInstruction->imm = 0;
        } else {      //j
            token2 = strtok(NULL, ",\t\n ");
            int index = -1;
            for (int i = 0; i < symbolTabLength; i++) {
                if (strcmp(symbolT[i].symbol, token2) == 0) {
                    index = i;
                }
                if (index != -1) {
                    currentInstruction->imm = symbolT[index].value;
                } else if (isalpha(token2[0])) {
                    puts("ERROR! Invalid Label\n");
                    exit(1);
                } else {
                    long num = atol(token2);
                    if (num < lineNumber) {
                        currentInstruction->imm = num;
                    } else {
                        puts("ERROR! exit(1) : invalid Offset\n");
                        exit(1);
                    }
                }
            }
        }
        currentInstruction->PC = currentInstruction->PC + 1;
        currentInstruction->inst[0] = '0';
        currentInstruction->inst[1] = hexTable[op];
        currentInstruction->inst[2] = '0';
        currentInstruction->inst[3] = '0';
        int2hex16(lower, currentInstruction->imm);
        currentInstruction->inst[4] = lower[0];
        currentInstruction->inst[5] = lower[1];
        currentInstruction->inst[6] = lower[2];
        currentInstruction->inst[7] = lower[3];
        currentInstruction->inst[8] = '\0';
        int res = hex2int(currentInstruction->inst);
        currentInstruction->intInst = res;
    }
}



int hex2int( char* hex)
{
    int result=0;
    while ((*hex)!='\0')
    {
        if (('0'<=(*hex))&&((*hex)<='9'))
            result = result*16 + (*hex) -'0';
        else if (('a'<=(*hex))&&((*hex)<='f'))
            result = result*16 + (*hex) -'a'+10;
        else if (('A'<=(*hex))&&((*hex)<='F'))
            result = result*16 + (*hex) -'A'+10;
        hex++;
    }
    return(result);
}
void int2hex16(char *lower,int a){
    sprintf(lower,"%X",a);
    if(a <0x10){
        lower[4]='\0';
        lower[3]=lower[0];
        lower[2]='0';
        lower[1]='0';
        lower[0]='0';
    }
    else if(a <0x100){
        lower[4]='\0';
        lower[3]=lower[1];
        lower[2]=lower[0];
        lower[1]='0';
        lower[0]='0';
    }
    else if(a <0x1000){
        lower[4]='\0';
        lower[3]=lower[2];
        lower[2]=lower[1];
        lower[1]=lower[0];
        lower[0]='0';
    }
}
void duplicatedLabel(struct symbolTable *symbolT,int symbolTabLength){
    Boolean duplicated=FALSE;
    for (int i = 0; i < symbolTabLength; ++i) {
        for (int j = i+1; j < symbolTabLength; ++j) {
            if(strcmp(symbolT[i].symbol,symbolT[j].symbol)==0){
                duplicated=TRUE;
                break;
            }
        }
    }
    if(duplicated){
        puts("ERROR! Duplicated Label\n");
        exit(1);
    }
}
int validOpCode(char *token2,char *instructions[]){
    int opcode=-1;
    for (int op = 0; op < 15; op++) {
        if (strcmp(instructions[op], token2) == 0){
            opcode=op;
            break;
        }
    }
    return opcode;
}