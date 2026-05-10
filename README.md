# crackme_reversing
Reverse engineering LaFarge's Crackme v0.2.

LaFarge's Crackme v0.2: https://crackmes.one/crackme/5ab77f5633c5d40ad448c2f0

# Solution
This is my first crackme. I first tried opening it in Ghidra to perform a static analysis, but the "don't look at my code" part seems to mess with Ghidra. I believe it’s some kind of obfuscation, but I don’t know exactly how it works. I found it very interesting, but I wasn’t making any progress in Ghidra, so I decided to switch to Binary Ninja which worked.

I started by searching string references to get to the code where the error message was displayed. From there I found the "magic" value very easily in the decompiled code but couldn't figure out exactly how it worked. I started the debugger in Binary Ninja and entered a username to observe what the code does. If we input a username with fewer than 4 characters, we get an error message.

Next, I found the loop responsible for generating the actual key. It iterates up to the maximum length between the username and the magic string. First, it copies the magic into the "key array", then starts iterating over both the key array and the username. It performs an XOR between them, takes the result modulo 0x19, and then adds 0x41 to it. The result is written back into the key array.

After the loop finishes, the key is formatted into the serial format: XXXX-XXXX-XXXX-XXXX.

The keygen logic was straightforward. However, when I tested with usernames longer than the magic string, it didn’t work. That’s when I realized that the loop iterates based on the maximum length between the username and the magic.

Anyways, it was a great exercise. I had a lot of fun doing it.

# Build
gcc cmd-keygen.c -o cmd-keygen
gcc gui-keygen.c -o gui-keygen -mwindows
