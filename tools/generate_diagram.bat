@echo off

IF NOT EXIST ..\docs\html mkdir ..\docs\html
	
dot.exe -Tpng -o ..\docs\html\jz.png -Kdot jz.dot
