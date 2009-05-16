@echo off
call "%VS90COMNTOOLS%vsvars32.bat" > build_jz.log

echo Building jz...
cd   jz\vc9

"%VS90COMNTOOLS%..\IDE\VCExpress.exe" /Build Debug   jz.sln >> build_jz.log
"%VS90COMNTOOLS%..\IDE\VCExpress.exe" /Build Profiling jz.sln >> build_jz.log
"%VS90COMNTOOLS%..\IDE\VCExpress.exe" /Build Release jz.sln >> build_jz.log
