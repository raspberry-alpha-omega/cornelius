.globl	saveRegs
.globl	restoreRegs
.globl	contextSwitch


@
@   Saves all the registers except those associated with the stack and program
@   counter.
@   @param r0 block to save registers to.
@
saveRegs:
	stm		r0,{r0-r15}
	bx 		lr

@
@  Restores the registers except those associated with the stack and program
@  counter
@  @param r0 block to get th registers from
@
restoreRegs:
	ldm		r0,{r0-r12}
	bx		lr

@
@   Perform a context switch.  This means saving all the registers to the
@   from block and then retrieving the registers from the to block.  The one
@   register we do not change is the program counter because, if this is the
@   only place  we do a context switch, it is guaranteed to be correct already.
@
@   @param r0 from register block
@   @param r1 to register block
@
contextSwitch:
	stm		r0,{r0-r15}
	ldm		r1,{r0-r14}
	bx		lr

@ last line
