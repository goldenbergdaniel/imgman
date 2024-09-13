@echo off
setlocal

set SOURCES= stb_image.h stb_image_write.h
set DEFINES= /DSTB_IMAGE_IMPLEMENTATION /DSTB_IMAGE_WRITE_IMPLEMENTATION

cl /c /EHsc /O2 /TC %DEFINES% %SOURCES%
lib *.obj /OUT:stbi.lib
del *obj
