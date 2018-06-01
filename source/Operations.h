#include "VirtualMachine.h"

#ifndef OPERATIONS_HEADER
#define OPERATIONS_HEADER

int opLiteral(CPU*);
int opReturn(CPU*);
int opLoad(CPU*);
int opStore(CPU*);
int opCall(CPU*);
int opAllocate(CPU*);
int opJump(CPU*);
int opConditionalJump(CPU*);
int opSystemCall(CPU*);
int opNegate(CPU*);
int opAdd(CPU*);
int opSubtract(CPU*);
int opMultiply(CPU*);
int opDivide(CPU*);
int opIsOdd(CPU*);
int opModulus(CPU*);
int opIsEqual(CPU*);
int opIsNotEqual(CPU*);
int opIsLessThan(CPU*);
int opIsLessThanOrEqualTo(CPU*);
int opIsGreaterThan(CPU*);
int opIsGreaterThanOrEqualTo(CPU*);
#endif
