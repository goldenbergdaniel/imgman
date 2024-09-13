#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "base/base.hpp"
#include "base/base_arena.cpp"
#include "base/base_string.cpp"
#include "arguments.cpp"
#include "image.cpp"
#include "manipulator.cpp"

i32 main(i32 argc, char *argv[])
{
  Arena arena = create_arena(MiB(16), false);

  Image image;
  image.read_from_path(str("res/car.tga"), &arena);
  image.add(10, Channel_R | Channel_B);
  printf("W: %i\n", image.width);
  printf("H: %i\n", image.height);

  Image overlay;
  overlay.read_from_image(&image, &arena);

  Slice<u8> bytes;
  bytes.from_ptr(image.raw_data(), 0, image.size());
  bytes = bytes.slice(0, bytes.len());
  printf("%i %i %i\n", bytes[0], bytes[1], bytes[2]);

  arena.clear();

  String name = str("Daniel");
  printf("%c\n", name[0]);
  String res = name.slice(1, name.len()-1);
  res = res.clone(&arena);
  printf("%s\n", res.raw_data());
  
  return 0;
}
