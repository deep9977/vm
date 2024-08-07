#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

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
    size_t program_size;
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

};
size_t Program_Size = sizeof(Program)/sizeof(Program[0]);

VM vm = {0};

size_t get_file_size(FILE *filefd){

    if(fseek(filefd, 0, SEEK_END)){
        perror("fseek1");
        exit(EXIT_FAILURE);
    }
    long file_size = ftell(filefd);
    if(file_size <= 0){
        perror("ftell");
        exit(EXIT_FAILURE);
    }
    if(fseek(filefd, 0, SEEK_SET)){
        perror("fseek2");
        exit(EXIT_FAILURE);
    }    
    return file_size;
}

void load_line(Inst inst, VM *vm){
    vm -> program[vm -> program_size] = inst;
    vm -> program_size++;
}

Inst_Type cstr_to_inst(char *str){
    if(strcmp(str, "push") == 0) return INST_PUSH; 
    if(strcmp(str, "pop") == 0) return INST_POP; 
    if(strcmp(str, "add") == 0) return INST_ADD;
    if(strcmp(str, "sub") == 0) return INST_SUB; 
    if(strcmp(str, "mul") == 0) return INST_MUL; 
    if(strcmp(str, "divi") == 0) return INST_DIVI; 
    if(strcmp(str, "jmp") == 0) return INST_JMP; 

    fprintf(stderr, "unknown instruction\n");
    exit(EXIT_FAILURE);
}

Inst translate_line(char *line, VM *vm){
    size_t line_size = strlen(line);
    char str[5] = {0}; 
    int operand = 0;

    int ssf = sscanf(line, "%4s %d", &str, &operand);
    Inst inst = {
        .type = cstr_to_inst(str),
        .operand = operand
    };

    load_line(inst, vm);
}

void translate_asm(char *source, VM *vm){
    size_t source_size = strlen(source);

    while(source_size > 0){
        char *end = memchr(source, '\n', source_size);
        char *line = strtok(source, "\n");
        if(line != NULL) translate_line(line, vm);

        if(end != NULL){
            size_t n = (size_t)(end - source);
            source_size -= n + 1;
            source = end + 1;
        }else{
            source = end ;
            source_size = 0 ;
        }
    
    }
}

void load_asm_from_file(char *path, VM *vm){
    FILE *filefd = fopen(path, "r");
    if(!filefd){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    size_t file_size = get_file_size(filefd);
    
    char buffer[file_size];
    int r = fread(buffer, file_size, 1, filefd);
    if(r == -1){
        perror("read");
        exit(EXIT_FAILURE);
    }


    translate_asm(buffer, vm);

}

void write_program_to_file(VM vm ){
    FILE *filefd;
    filefd = fopen("./prog.vm", "wb" );
    if(!filefd){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int fw = fwrite(vm.program, sizeof(vm.program[0]), vm.program_size, filefd);
    fclose(filefd);
    if(fw != vm.program_size){
        perror("fwrite");
        exit(EXIT_FAILURE);
    }
}

int load_program_from_file(VM *vm){
    FILE *filefd;
    filefd = fopen("./prog.vm", "rb" );
    if(!filefd){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    size_t file_size = get_file_size(filefd);

    if((file_size % sizeof(Inst)) != 0 ){
        fprintf(stderr, "problem with .vm file");
        exit(EXIT_FAILURE);
    }
    vm->program_size = file_size/sizeof(Inst);

    int fr = fread(vm->program, vm->program_size, sizeof(Inst), filefd);
    if(fr <= 0){
        perror("fread");
        exit(EXIT_FAILURE);
    }

}

int load_program_from_memory(VM *vm){
    if(Program_Size > VM_INST_CAPACITY) return EXIT_FAILURE; 

    memcpy(vm->program, Program, sizeof(Program[0]) * Program_Size);
    vm->program_size = Program_Size;
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
            if(inst.operand > vm->program_size || inst.operand < 0) return TRAP_INVALID_JMP;

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


int main2(void){

    load_asm_from_file("./prog.vasm", &vm);
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


void main(){
    load_asm_from_file("./prog.vasm", &vm);
    main2();


}