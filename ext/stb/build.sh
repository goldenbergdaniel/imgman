SOURCES="stb_image.h stb_image_write.h"
DEFINES="-DSTB_IMAGE_IMPLEMENTATION -DSTB_IMAGE_WRITE_IMPLEMENTATION"

cc -x c $SOURCES $DEFINES -O2 -c
ar -crs libstbi.a *.o
rm -f *.o
