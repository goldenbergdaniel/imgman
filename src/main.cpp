#include "base/base.hpp"
#include "base/base_os.cpp"
#include "base/base_arena.cpp"
#include "base/base_string.cpp"
#include "arguments.cpp"
#include "image.cpp"
#include "manipulator.cpp"

i32 main(i32 argc, char *argv[])
{
  Arena arena = Arena(GiB(1));

  Image image;
  image.read_from_path(str("res/alpha.tga"), &arena);
  image.add(10, Channel_R | Channel_B);
  image.print_stats();

  Slice<byte> bytes;
  bytes.from_ptr(image.raw_data(), 0, image.size());
  bytes = bytes.slice(0, bytes.len());
  printf("   PIXEL: %i %i %i %i\n", bytes[0], bytes[1], bytes[2], bytes[3]);

  arena.clear();

  if (argc > 1)
  {
    String str = string_from_argv(argv, argc, &arena);
    Slice<Argument> args = arguments_from_string(str, &arena);

    for (i32 i = 0; i < args.len(); i++)
    {
      args[i].str.print();
    }
  }
  
  return 0;
}
