[BotW_SpawnActors_V208]
moduleMatches = 0x6267BFD0


0x037b6040 = FunctionToJump:

.origin = codecave

startData: ; where our data starts

; Whether our function should be called
Enabled: 
.int 0

; All of our registers to back up
O_R3:
.int 0
O_R4:
.int 0
O_R5:
.int 0
O_R6:
.int 0
O_R7:
.int 0
O_R8:
.int 0
O_R9:
.int 0
O_R10:
.int 0
O_LR: ; ...Including the LR register
.int 0

; All of our registers to overwrite with
N_R3:
.int 0
N_R4:
.int 0
N_R5:
.int 0
N_R6:
.int 0
N_R7:
.int 0
N_R8:
.int 0
N_R9:
.int 0
N_R10:
.int 0

CallFunction:
; r14 happens to be 0 here, so this works fine.
; I'm sure there's a better way of doing this,
; but whatever.

; Backup registers
lis r14, O_R3@ha
stw r3, O_R3@l(r14)

lis r14, O_R4@ha
stw r4, O_R4@l(r14)

lis r14, O_R5@ha
stw r5, O_R5@l(r14)

lis r14, O_R6@ha
stw r6, O_R6@l(r14)

lis r14, O_R7@ha
stw r7, O_R7@l(r14)

lis r14, O_R8@ha
stw r8, O_R8@l(r14)

lis r14, O_R9@ha
stw r9, O_R9@l(r14)

lis r14, O_R10@ha
stw r10, O_R10@l(r14)

lis r14, O_LR@ha
mflr r3 ; whatever, we've already backed up r3 so it's usable
stw r3, O_LR@l(r14) ; LR too

; Our custom stuff
; -----------------------------------

; Lets call the function from the dll
; so that we can get stuff to call with.
lis r3, startData@ha
addi r3, r3, startData@l
bla import.coreinit.fnCallMain





; Set up where to jump to...
lis r3, FunctionToJump@ha
addi r3, r3, FunctionToJump@l
mtctr r3

; ... and where to jump back to
lis r3, restoreAndExit@ha
addi r3, r3, restoreAndExit@l
mtlr r3


; Set up if statment
lis r14, Enabled@ha
lwz r3, Enabled@l(r14)
cmpwi r3, 0x0
beq restoreAndExit ; We can skip over applying params if we're not calling the func, so putting this here is fine.

; Set stuff to false so we don't infinitly call this
li r3, 0x0
lis r14, Enabled@ha
stw r3, Enabled@l(r14)

;; Apply our params
lis r14, N_R3@ha
lwz r3, N_R3@l(r14)

lis r14, N_R4@ha
lwz r4, N_R4@l(r14)

lis r14, N_R5@ha
lwz r5, N_R5@l(r14)

lis r14, N_R6@ha
lwz r6, N_R6@l(r14)

lis r14, N_R7@ha
lwz r7, N_R7@l(r14)

lis r14, N_R8@ha
lwz r8, N_R8@l(r14)

lis r14, N_R9@ha
lwz r9, N_R9@l(r14)

lis r14, N_R10@ha
lwz r10, N_R10@l(r14)


; Call the function
bctr

restoreAndExit:



; -----------------------------------

; Restore registers

lis r14, O_LR@ha
lwz r3, O_LR@l(r14) ; Restore LR as well
mtlr r3

lis r14, O_R3@ha
lwz r3, O_R3@l(r14)

lis r14, O_R4@ha
lwz r4, O_R4@l(r14)

lis r14, O_R5@ha
lwz r5, O_R5@l(r14)

lis r14, O_R6@ha
lwz r6, O_R6@l(r14)

lis r14, O_R7@ha
lwz r7, O_R7@l(r14)

lis r14, O_R8@ha
lwz r8, O_R8@l(r14)

lis r14, O_R9@ha
lwz r9, O_R9@l(r14)

lis r14, O_R10@ha
lwz r10, O_R10@l(r14)

li r14, 0x0


; Do what we were replacing
lwz r3, 0x30(r3)


; Branch to the next instruction after this is called..
; This function only gets called from one location so this is fine.
b 0x0313ea78 




0x0313ea74 = b CallFunction ; Send stuff over to our custom function