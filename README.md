# SpawnActors
This project has the aim of making actor creation functions accessible from an injected DLL.
Huge thank-you to [PointCrow](https://twitch.tv/PointCrow) for funding this project!

## Usage
#### Note:
It's a coin toss for whether or not your antivirus will detect this as a virus, since antiviruses hate dll injectors. Good luck, and have fun making exclusions!
### From Prebuild Binary
Simply download a prebuilt binary and run the executable while cemu is on the title select screen.
### Building from Source
Build the solution with Visual Studio, and run the outputted executable.

## How it Works
An assembly patch is applied via a Cemu graphics pack. It replaces a vanilla instruction (which is executed on a regular basis when the game is unpaused) with PPC Assembly that generally corresponds to the following pseudocode:
```cpp
// Back up registers
backup_r3 = r3;
backup_r4 = r4;
backup_r5 = r5;
backup_r6 = r6;
backup_r7 = r7;
backup_r8 = r8;
backup_r9 = r9;
backup_r10 = r10;

// Call function from DLL
// This function should set the new_r* variables, as well as an "enabled" variable.
dllFunc();

if (enabled) {
  // Set the params that the desired vanilla function will take
  r3 = new_r3;
  r4 = new_r4;
  r5 = new_r5;
  r6 = new_r6;
  r7 = new_r7;
  r8 = new_r8;
  r9 = new_r9;
  r10 = new_r10;
  // Call our the desired vanilla function
  vanillaFunction();
}

// Restore old registers
r3 = backup_r3;
r4 = backup_r4;
r5 = backup_r5;
r6 = backup_r6;
r7 = backup_r7;
r8 = backup_r8;
r9 = backup_r9;
r10 = backup_r10;
```
This is an oversimplification, but it's good enough for documentation purposes. One that wishes to understand it further can simply look at the asm patch. Note that the patch only supports registers 3-10. These tend to be the only ones used for passing params to functions ([See here](https://www.cs.uaf.edu/2010/fall/cs301/lecture/12_01_PowerPC.html)).

The DLL function (called ``dllFunc()`` in the above example) mostly sets params to values that have previously been seen to work well. It does this by copying values to memory allocated via the assembly patch.
