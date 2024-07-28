#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define VM_STACK_CAPACITY 512 
#define VM_INST_CAPACITY 1024


typedef enum{
    TRAP_OK,
    TRAP_STACK_OVERFLOW,
    TRAP_STACK_UNDERFLOW,
    TRAP_INVALID_JMP,
    TRAP_DIVI_BY_ZERO,
}Trap;

typedef enum {
    INST_PUSH,
    INST_POP,
    INST_ADD,
    INST_SUB,
    INST_MUL,
    INST_DIVI,
    INST_JMP,

}Inst_Type;

typedef struct{
    Inst_Type type;
    int operand;
}Inst;

typedef struct{
    int stack[VM_STACK_CAPACITY];
    int sp;
    Inst program[VM_INST_CAPACITY];
    int program_size;
    int ip;
}VM;


#define inst_push(operand1) ((Inst){.type = INST_PUSH, .operand = operand1}) 
#define inst_pop() ((Inst){.type = INST_POP}) 
#define inst_add() ((Inst){.type = INST_ADD})
#define inst_sub() ((Inst){.type = INST_SUB})
#define inst_mul() ((Inst){.type = INST_MUL})
#define inst_divi() ((Inst){.type = INST_DIVI})
#define inst_jmp(operand1) ((Inst){.type = INST_JMP, .operand = operand1})
Inst Program[]= {
    inst_push(69),
    inst_push(420),
    inst_add(),
    inst_push(3),
    inst_push(2),
    inst_mul(),

};
int Program_Size = sizeof(Program)/sizeof(Program[0]);

VM vm = {0};

int load_program_from_memory(VM *vm){
    if(Program_Size > VM_INST_CAPACITY) return EXIT_FAILURE; 

    memcpy(vm->program, Program, sizeof(Program[0]) * Program_Size);
    vm->program_size = Program_Size;
}

int write_program_to_file(VM vm){
    
}

Trap vm_execute_inst(VM *vm, Inst inst){

    switch(inst.type){

        case INST_PUSH:      
            if(vm->sp >= VM_STACK_CAPACITY ) return TRAP_STACK_OVERFLOW;

            vm->stack[vm->sp] = inst.operand;
            vm->sp++;   
            break;
        
        case INST_POP:
            if(vm->sp <= 0) return TRAP_STACK_UNDERFLOW;
            
            vm->sp--;
            break;
        
        case INST_ADD:
            if((vm->sp-2) < 0 ) return TRAP_STACK_UNDERFLOW;
            
            vm->stack[vm->sp-2] += vm->stack[vm->sp-1 ];
            vm->sp -= 1 ;
            break;
        
        case INST_SUB:
            if((vm->sp-2) < 0 ) return TRAP_STACK_UNDERFLOW;

            vm->stack[vm->sp-2] -= vm->stack[vm->sp-1 ];
            vm->sp -= 1 ;
            break;
    
        case INST_MUL: 
            if((vm->sp-2) < 0 ) return TRAP_STACK_UNDERFLOW;

            vm->stack[vm->sp-2] *= vm->stack[vm->sp-1 ];
            vm->sp -= 1 ;
            break;
        
        case INST_DIVI:
            if((vm->sp-2) < 0 ) return TRAP_STACK_UNDERFLOW;
            if((vm->stack[vm->sp-1]) == 0 ) return TRAP_DIVI_BY_ZERO;

            vm->stack[vm->sp-2] /= vm->stack[vm->sp-1 ];
            vm->sp -= 1 ;
            break;
        
        case INST_JMP: 
            if(inst.operand > Program_Size || inst.operand < 0) return TRAP_INVALID_JMP;

            vm->ip = inst.operand;
            return TRAP_OK;
        
    
    }
    vm->ip++;
    return TRAP_OK;
}

void dump_mem(VM vm){
    for(int i = 0 ; i < vm.sp ; i++ ){
        printf("%d\n",vm.stack[i]);
    }
}


int main(){
    load_program_from_memory(&vm);
    while(vm.ip < vm.program_size && vm.ip >= 0){
        Trap trap = vm_execute_inst(&vm ,vm.program[vm.ip]);
        if(trap != TRAP_OK){
            printf("[ERROR]: %d",trap);
            return EXIT_FAILURE;
        }
    }
    
    dump_mem(vm);


    return EXIT_SUCCESS;
}
