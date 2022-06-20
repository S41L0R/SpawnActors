[Botw_SpawnActors_V208]
moduleMatches = 0x6267BFD0

.origin = codecave

HLE_OnDamage_Loc:
.int 0

HLE_OnDamage_Name:
.string "fnOnDamage"



OnDamage:
; Backup registers
addi r1, r1, -0x18
stw r3, 0(r1)
stw r4, 4(r1)
stw r5, 8(r1)
stw r6, 12(r1)
mfctr r3
stw r3, 16(r1)
mflr r3
stw r3, 20(r1)

; Check if we already have our dll func addr
lis r3, HLE_OnDamage_Loc@ha
lwz r3, HLE_OnDamage_Loc@l(r3)
cmpwi r3, 0x0
bne HLE_OnDamage_Call

; Get all sorts of stuff if not
lis r3, ModuleName@ha
addi r3, r3, ModuleName@l
lis r4, ModuleHandle@ha
addi r4, r4, ModuleHandle@l
bla import.coreinit.OSDynLoad_Acquire

cmpwi r3, 0x0 ; Anything other than 0x0 is an error code.. 
bne HLE_OnDamage_RestoreAndExit ; in this case it means that the module isn't registered (yet).

lis r3, ModuleHandle@ha
lwz r3, ModuleHandle@l(r3)
li r4, 0x0
lis r5, HLE_OnDamage_Name@ha
addi r5, r5, HLE_OnDamage_Name@l
lis r6, HLE_OnDamage_Loc@ha
addi r6, r6, HLE_OnDamage_Loc@l
bla import.coreinit.OSDynLoad_FindExport

HLE_OnDamage_Call:
lis r3, HLE_OnDamage_Loc@ha
lwz r3, HLE_OnDamage_Loc@l(r3)
mtctr r3
bctrl

HLE_OnDamage_RestoreAndExit:
lwz r3, 16(r1)
mtctr r3
lwz r3, 20(r1)
mtlr r3
lwz r3, 0(r1)
lwz r4, 4(r1)
lwz r5, 8(r1)
lwz r6, 12(r1)
addi r1, r1, 0x18

stw r31, 0x0(r28) ; original instruction

b 0x02D452A8

0x02D452A4 = b OnDamage