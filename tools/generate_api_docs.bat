@echo off
echo Generating API docs...once complete, HTML Help Workshop can be used to generate a compiled Windows help file. Open doc\html\index.hhp and compiled it.
call generate_diagram.bat > doc_build.log
doxygen.exe jz.cfg >> doc_build.log
