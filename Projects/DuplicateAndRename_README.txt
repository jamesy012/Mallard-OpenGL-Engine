DuplicateAndRename will copy a folder and it's contents to another new folder
It will rename the 3 *.vcxproj files to have the name of the new folder

This was made to dupliate projects for visual studio

USAGE
------------
ALL FILES ARE RELATIVE TO THE .EXE

by running DuplicateAndRename.exe it will open a console window
you enter the name of the new folder
and it will copy contents of the folder listed in the DuplicateAndRenameFromFile.txt
into the folder you named in the folder

to change the from folder change the first line in the DuplicateAndRenameFromFile.txt
(it only reads the first line)

NOTES
-----------
if you try to use this on a folder that is already created and has the desired files
it will as if you want to copy over and duplicate but will also add another set of *.vcxproj files
since it cant rename to a file thats already there