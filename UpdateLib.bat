pushd %~dp0

xcopy /y/s	.\Engine\public\*.*				.\Reference\Headers\
xcopy /y/s	.\Engine\bin\*.lib				.\Reference\Librarys\

xcopy /y/s	.\Engine\bin\*.dll				.\Client\Bin\
xcopy /y/s	.\Engine\bin\ShaderFiles\*.*	.\Client\Bin\ShaderFiles\
