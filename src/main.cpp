#include <fstream>
#include <cstring>

#include "globals.hpp"
#include "util.hpp"
#include "image.cpp"

bool ends_with_tga(char *name, i8 len);

i32 main(i32 argc, char *argv[])
{
  i32 data[1024] = {};

  Slice<i32> nums;
  nums.from_ptr(data, 0, 5);
  nums[3] = 3;

  Slice<i32> temp = nums.slice(2, nums.len());
  printf("%i\n", temp[1]);

  return 0;

  Image track_image;

  std::ifstream f_track;
  std::ofstream f_out;

  if (argc > 3)
  {
    char *out_name = argv[1];
    u64 len = strlen(out_name);
    
    if (ends_with_tga(out_name, len))
    {
      f_out.open(out_name, std::ios::binary);
    }
    else
    {
      printf("Invalid file name.\n");
      return 1;
    }

    char *track_name = argv[2];
    len = strlen(track_name);
    
    if (ends_with_tga(track_name, len))
    {
      f_track.open(track_name, std::ios::binary);

      if (f_track.is_open())
      {
        track_image.read(&f_track);
      }
      else
      {
        printf("TGAmod: File does not exist.\n");
        return 1;
      }
    }
    else
    {
      printf("TGAmod: Invalid file name.\n");
      return 1;
    }
  }
  else if (argc == 1)
  {
    if (strcmp(argv[1], "--help"))
    {
      // print help
    }

    return 0;
  }
  else
  {
    printf("TGAmod: Invalid file name.\n");
    return 1;
  }
  
  return 0;
}

bool ends_with_tga(char *name, i8 len)
{
  return name[len-1] == 'a' && name[len-2] == 'g' && name[len-3] == 't' && name[len-4] == '.';
}
