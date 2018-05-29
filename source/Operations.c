#include <stdio.h>
#include "VirtualMachine.h"

// NONE OF THESE ARE SAFE AND CAN DEREFERENCE NULL


int opLiteral(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = cpu->instRegister.MField;
    
    return OP_SUCCESS;
}

int opReturn(CPU *cpu) {
}

int opLoad(CPU *cpu) {
}

int opStore(CPU *cpu) {
}

int opCall(CPU *cpu) {
}

int opAllocate(CPU *cpu) {
}

int opJump(CPU *cpu) { // DONE
    cpu->programCounter = cpu->instRegister.MField;

    return OP_SUCCESS;
}

int opConditionalJump(CPU *cpu) { // DONE
    if (cpu->registers[cpu->instRegister.RField] == 0) {
        cpu->programCounter = cpu->instRegister.MField;
    }

    return OP_SUCCESS;
}

int opSystemCall(CPU *cpu) { // DONE
    switch (cpu->instRegister.MField) {
        case 1:
            printf("%d\n", cpu->registers[cpu->instRegister.RField]);
            return OP_SUCCESS;
    
        case 2:// not super safe oh well // might not dereference correctly
            scanf("%d", &cpu->registers[cpu->instRegister.RField]);
            return OP_SUCCESS;
       
        case 3:
            return KILL_PROGRAM;// some kill command     
        
        default:
            return OP_FAILURE;//fail
    }
}

int opNegate(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = 0 - cpu->registers[cpu->instRegister.LField];

    return OP_SUCCESS;
}

int opAdd(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] +
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

int opSubtract(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] -
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

int opMultiply(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] *
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

int opDivide(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] /
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

int opIsOdd(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.RField] % 2;

    return OP_SUCCESS;
}

int opModulus(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = cpu->registers[cpu->instRegister.LField] %
                                               cpu->registers[cpu->instRegister.MField];
    return OP_SUCCESS;
}

int opIsEqual(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] ==
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

int opIsNotEqual(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] !=
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

int opIsLessThan(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] <
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

int opIsLessThanOrEqualTo(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] <=
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

int opIsGreaterThan(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] >
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}

int opIsGreaterThanOrEqualTo(CPU *cpu) { // DONE
    cpu->registers[cpu->instRegister.RField] = (cpu->registers[cpu->instRegister.LField] >=
                                                cpu->registers[cpu->instRegister.MField]);
    return OP_SUCCESS;
}
