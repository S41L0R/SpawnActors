[BotW_SpawnActors_V208]
moduleMatches = 0x6267BFD0

.origin = codecave

; A whole bunch of general storage we can use for whatever we want
; 1024 bytes, to be specific.
; Broken up into 8 byte sections. (The biggest cemu will allow other than strings.)
StorageStart:
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
.double 0
StorageEnd:

startData: ; where our transferrable data starts

; Whether our function should be called
Enabled: 
.byte 0

InterceptRegisters:
.byte 0

FunctionToJump:
.int 0

RingPtr:
.int StorageStart

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


CallFunction:
; r14 happens to be 0 here, so this works fine.
; I'm sure there's a better way of doing this,
; but whatever.

; Backup registers
addi r1, r1, -0x30

stw r3, 0(r1)
stw r4, 4(r1)
stw r5, 8(r1)
stw r6, 12(r1)
stw r7, 16(r1)
stw r8, 20(r1)
stw r9, 24(r1)
stw r10, 28(r1)
stw r11, 32(r1) ; We don't need these for params, just need them freed for general register manipulation
stw r12, 36(r1)
mflr r3 ; whatever, we've already backed up r3 so it's usable
stw r3, 40(r1)
mfctr r3
stw r3, 44(r1)

; Our custom stuff
; -----------------------------------
; Lets call the function from the dll
; so that we can get stuff to call with.

lis r3, startData@ha
addi r3, r3, startData@l
lis r4, StorageStart@ha
addi r4, r4, StorageStart@l
lis r5, StorageEnd@ha
addi r5, r5, StorageEnd@l
bla import.coreinit.fnCallMain


; Set up if statment
lis r11, Enabled@ha
lbz r11, Enabled@l(r11)
cmpwi r11, 0x0
beq restoreAndExit ; We can skip over applying params if we're not calling the func, so putting this here is fine.

; Set up where to jump to...
lis r11, FunctionToJump@ha
lwz r11, FunctionToJump@l(r11)
mtctr r11

; ... and where to jump back to
lis r11, restoreAndExit@ha
addi r11, r11, restoreAndExit@l
mtlr r11


; Set stuff to false so we don't infinitly call this
li r11, 0x0
lis r12, Enabled@ha
stb r11, Enabled@l(r12)

; Call the function
bctr

restoreAndExit:
; -----------------------------------

; Restore registers

; We use r3 for this, so let's do it before we restore r3!
lwz r3, 44(r1)
mtctr r3

lwz r3, 40(r1)
mtlr r3

; Start GPR restore
lwz r3, 0(r1)
lwz r4, 4(r1)
lwz r5, 8(r1)
lwz r6, 12(r1)
lwz r7, 16(r1)
lwz r8, 20(r1)
lwz r9, 24(r1)
lwz r10, 28(r1)
lwz r11, 32(r1)
lwz r12, 36(r1)

addi r1, r1, 0x30



; Do what we were replacing
cmpwi r3, 0x0


; Branch to the next instruction after this is called..
b 0x0313b844




0x0313b840 = b CallFunction ; Send stuff over to our custom function



; Register finder
; ----------------------------------------------

0x037b6040 = b GetTargetFnRegisters

GetTargetFnRegisters:

; Back up registers we use
addi r1, r1, -0x8; Give us some heap space to work with
stw r0, 0(r1)
stw r14, 4(r1)

; Set up if statment
lis r14, InterceptRegisters@ha
lbz r0, InterceptRegisters@l(r14)
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
stb r0, InterceptRegisters@l(r14)

InterceptRestoreAndExit:

; Restore registers we used
lwz r0, 0(r1)
lwz r14, 4(r1)
addi r1, r1, 0x8; Move the heap pointer back!

; original instruction:
stwu r1, -0x38(r1)

; Return
b 0x037b6044

; ----------------------------------------------