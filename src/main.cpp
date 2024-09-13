#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "base/base.hpp"
#include "base/base_arena.cpp"
#include "base/base_string.cpp"
#include "image.cpp"

i32 main(i32 argc, char *argv[])
{
  i32 width, height, channels;
  u8 *data = stbi_load("res/car.tga", &width, &height, &channels, 3);
  printf("W: %i\n", width);
  printf("H: %i\n", height);

  Slice<u8> bytes;
  bytes.from_ptr(data, 0, width*height);
  bytes = bytes.slice(0, bytes.len());
  printf("%i %i %i\n", bytes[0], bytes[1], bytes[2]);

  Arena arena = create_arena(1024, false);

  String name = str("Daniel");
  printf("%c\n", name[0]);
  String res = name.slice(1, name.len()-1);
  res = res.clone(&arena);
  printf("%s\n", res.raw_data());

  arena.clear();
  
  return 0;
}
