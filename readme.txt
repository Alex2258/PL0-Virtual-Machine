Designed ENTIRELY BY: ------ALEX ARWIN-------

This PL/0 Virtual Machine runs as stated in the original "HW1 PL0 P-machine" docx
posted on webcourses. It implements all the requirements as directed and follows
all rules regarding the language and its ISA.

Input File: vminput.txt
Output File: vmoutput.txt

All three test cases work as inteded and provided. The input & output files 
match appropriately as does the terminal during execution as tested on my
personal computer AND Eustis.

This program works uniquely in that the method being used to print the 
data was carefully thought about done in an efficient manner. I decided
to print the data before the fetch (for the previous instruction), complete
the fetch, and proceed to print the fetched instruction data. So each
cycle of fetch prints out the current cylces bp,sp,stack and prints the next
cycles instruction #, opcode, l, m. Additionally, the method used to handle
the "|" between ARs is unique. I maintain an array of SP locations when CAL
is made. I think save that location and when I am printing the stack, I print 
an "|" at those saved locations. When a RET call is made, I remove the last
location stored to avoid printing "|"'s once the AR is removed.