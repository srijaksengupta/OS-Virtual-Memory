# OS-Virtual-Memory
### Built and OS Kernel with Virtual Memory based in C designed to support 6 types of commands which are defined below:

* **help:** Displays all the commands
* **quit:** Exits / terminates the shell with “Bye!”
* **set VAR STRING:** Assigns a value to shell memory
* **print VAR:** Displays the STRING assigned to VAR
* **run SCRIPT.TXT:** Executes the file SCRIPT.TXT
* **exec p1 p2 p3:** Executes either 1, 2 or 3 programs using the concept of execution for concurrent programs

### The virtual memory for this OS Kernel has been implemented by including the following elements:

The OS Boot Sequence to create some necessary OS structures 
* Prepare the BackingStore
* Prepare RAM for paging

The Memory Manager to handle memory allocation for processes 
* Launcher
  * Loads new programs to the run-time environment 
* PCB Modifications
  * Addition of the page table 
* Page Fault
  * Find page, swap in, select victim, we will not do a swap out 
* Task Switch Additions
  * Generate Page Fault and properly assigns addresses
