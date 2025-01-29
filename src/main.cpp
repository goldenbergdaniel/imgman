#include "base/base_os.cpp"
#include "base/base_arena.cpp"
#include "base/base_string.cpp"
#include "argument_parser.cpp"
#include "image.cpp"

i32 main(i32 argc, char *argv[])
{
  Arena perm_arena = Arena(GiB(1));
  init_scratches();

  Image target_image;
  target_image.read_from_file(str("res/car.tga"), &perm_arena);
  target_image.format_kind = Format_Kind_TGA;

  if (argc > 1)
  {
    String arg_str = string_from_argv(argv, argc, &perm_arena);
    Slice<Argument> args = arguments_from_string(arg_str, &perm_arena);
    for (u64 i = 0; i < args.len; i++)
    {
      // args[i].str.print();
    }

    for (u64 arg_idx = 0; arg_idx < args.len; arg_idx++)
    {
      Argument &arg = args[arg_idx];
      if (arg.kind == Argument_Kind_COMMAND)
      {
        if (arg.str == str("-add"))
        {
          Argument val = args[++arg_idx];
          if (val.kind != Argument_Kind_NUMBER)
          {
            printf("error: argument %i of command %i is not a number\n", 
                   arg.arg_idx, arg.cmd_idx);
            return 1;
          }

          // Argument ch = args[++arg_idx];
          // if (ch.kind != Argument_Kind_CHANNELS)
          // {
          //   printf("error: argument %i of command %i is not a channel set", 
          //          arg.arg_idx, arg.cmd_idx);
          //   return 1;
          // }

          target_image.add(val.value.number, Channel_R | Channel_B);
        }
      }
    }

    target_image.write_to_file(str("res/out.tga"));
  }
  else
  {
    printf("usage: imgman \'path/to/image\' [flags] [commands]\n");
  }
  
  return 0;
}
