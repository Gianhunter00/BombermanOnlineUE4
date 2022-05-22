## TXT FORMAT
TXT Format is pretty simple, it's a line of number divided by a "," the number decide which type of block to spawn:
- 0 Spawn a Floor
- 1 Spawn the Breakable block
- 2 Spawn the Unbreakable block
To make a new Row you can just create a newline and follow the same rule as before.
Example:
2,2,2,2,2,2
2,0,0,1,1,2
2,1,1,0,0,2
2,2,2,2,2,2

In this example the border will be all Unbreakable block and inside is a mix of only Floor and Breakable block
There is an example txt Map inside the folder Named "TXT MAP EXAMPLE"