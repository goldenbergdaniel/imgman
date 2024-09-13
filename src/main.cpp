#include "base/base.hpp"
#include "base/base_arena.cpp"
#include "base/base_string.cpp"
#include "arguments.cpp"
#include "image.cpp"
#include "manipulator.cpp"

i32 main(i32 argc, char *argv[])
{
  // init_scratch_arenas();

  Arena arena = Arena(MiB(16));

  Image image;
  image.read_from_path(str("res/car.tga"), &arena);
  image.add(10, Channel_R | Channel_B);
  image.print_stats();

  Image overlay;
  overlay.read_from_image(&image, &arena);

  Slice<u8> bytes;
  bytes.from_ptr(image.raw_data(), 0, image.size());
  bytes = bytes.slice(0, bytes.len());
  printf("%i %i %i\n", bytes[0], bytes[1], bytes[2]);

  arena.clear();

  String name = str("Daniel");
  printf("%c %llu\n", name[0], name.len()); 
  String res = name.slice(1, name.len()-1);
  res = res.clone(&arena);
  printf("%s\n", res.raw_data());

  if (argc > 1)
  {
    printf("\n");
    for (i32 i = 1; i < argc; i++) printf("%s ", argv[i]);
    printf("\n");
  }
  
  return 0;
}
