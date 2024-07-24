#include<stdio.h>
#include<stdlib.h>

#define VM_STACK_CAPACITY 512 


typedef struct{
    int stack[VM_STACK_CAPACITY];
    int sp;
    int ip;
}VM;

typedef enum {
    INST_PUSH,
    INST_POP,

}Inst_Type;

typedef struct{
    Inst_Type type;
    int operand;
}Inst;

void vm_execute_inst(VM *vm, Inst inst){

}

VM vm = {0};


int main(){



    return 0;
}