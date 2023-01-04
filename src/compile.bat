@echo on
cd ../data/shaders
for %%f in (*.frag,*.vert) do (
	%VULKAN_SDK%\Bin\glslc.exe "%%~f" -o "%%~f.spv"
)
cd ../../src