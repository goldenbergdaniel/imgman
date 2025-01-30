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
    Slice<Argument> arg_slc = arguments_from_string(arg_str, &perm_arena);
    for (u64 i = 0; i < arg_slc.len; i++)
    {
      // args[i].str.print();
    }

    Argument_Store args;
    args.data = arg_slc;

    // - Parser loop ---
    while (true)
    {
      Argument *arg = args.consume();
      if (arg->kind == Argument_Kind_COMMAND)
      {
        if (arg->str == str("-add"))
        {
          Argument *val = args.consume();
          if (val->kind != Argument_Kind_NUMBER)
          {
            printf("error: parameter %i of command %i must be a number\n", 
                   val->arg_pos, val->cmd_pos);
            return 1;
          }

          Argument *ch = args.consume();
          if (ch->kind != Argument_Kind_FLAGS)
          {
            printf("error: argument %i of command %i must be a channel set\n", 
                   ch->arg_pos, ch->cmd_pos);
            return 1;
          }

          Channel_Set channels = 0;
          for (u64 i = 0; i < ch->str.len(); i++)
          {
            char flag = ch->str[i];
            if (flag == 'R' || flag == 'r')
            {
              channels |= Channel_R;
            }
            else if (flag == 'G' || flag == 'g')
            {
              channels |= Channel_G;
            }
            else if (flag == 'B' || flag == 'b')
            {
              channels |= Channel_B;
            }
          }

          if (channels == 0)
          {
            channels = Channel_R | Channel_G | Channel_B;
          }

          target_image.add(val->value.number, channels);
        }
      }
      else if (arg->kind == Argument_Kind_NIL)
      {
        break;
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
