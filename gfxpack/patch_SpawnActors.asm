[BotW_SpawnActors_V208]
moduleMatches = 0x6267BFD0

.origin = codecave

startData: ; where our data starts

; Whether our function should be called
Enabled: 
.int 0

InterceptRegisters:
.int 0

FunctionToJump:
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

F_R3:
.int 0
F_R4:
.int 0
F_R5:
.int 0
F_R6:
.int 0
F_R7:
.int 0
F_R8:
.int 0
F_R9:
.int 0
F_R10:
.int 0

; A whole bunch of general storage we can use for whatever we want
; 256 bytes, to be specific.
; Broken up into 8 byte sections. (The biggest cemu will allow.)
STR_0:
.double 0
STR_1:
.double 0
STR_2:
.double 0
STR_3:
.double 0
STR_4:
.double 0
STR_5:
.double 0
STR_6:
.double 0
STR_7:
.double 0
STR_8:
.double 0
STR_9:
.double 0
STR_10:
.double 0
STR_11:
.double 0
STR_12:
.double 0
STR_13:
.double 0
STR_14:
.double 0
STR_15:
.double 0
STR_16:
.double 0
STR_17:
.double 0
STR_18:
.double 0
STR_19:
.double 0
STR_20:
.double 0
STR_21:
.double 0
STR_22:
.double 0
STR_23:
.double 0
STR_24:
.double 0
STR_25:
.double 0
STR_26:
.double 0
STR_27:
.double 0
STR_28:
.double 0
STR_29:
.double 0
STR_30:
.double 0
STR_31:
.double 0



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
lwz r3, FunctionToJump@l(r3)
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



; Register finder
; ----------------------------------------------

0x037b6040 = b GetTargetFnRegisters

GetTargetFnRegisters:

; Set up if statment
lis r14, InterceptRegisters@ha
lwz r0, InterceptRegisters@l(r14)
cmpwi r0, 0x0
beq InterceptRestoreAndExit ; We can skip over applying params if we're not calling the func, so putting this here is fine.

lis r14, F_R3@ha
stw r3, F_R3@l(r14)

lis r14, F_R4@ha
stw r4, F_R4@l(r14)

lis r14, F_R5@ha
stw r5, F_R5@l(r14)

lis r14, F_R6@ha
stw r6, F_R6@l(r14)

lis r14, F_R7@ha
stw r7, F_R7@l(r14)

lis r14, F_R8@ha
stw r8, F_R8@l(r14)

lis r14, F_R9@ha
stw r9, F_R9@l(r14)

lis r14, F_R10@ha
stw r10, F_R10@l(r14)

li r0, 0x0
lis r14, InterceptRegisters@ha
stw r0, InterceptRegisters@l(r14)

InterceptRestoreAndExit:

; original instruction:
stwu r1, -0x38(r1)

; Return
b 0x037b6044

; ----------------------------------------------