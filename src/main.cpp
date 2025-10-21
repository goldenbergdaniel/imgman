#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/param.h>

#include <map>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Base_Common ////////////////////////////////////////////////////////////////////////////


typedef uint8_t byte;
typedef uint8_t b8;
typedef uint16_t b16;
typedef uint32_t b32;
typedef uint64_t b64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;
typedef intptr_t iptr;
typedef uintptr_t uptr;
typedef const char *cstr;

#define size_of(T) sizeof(T)
#define align_of(T) alignof(T)


// Base_Arena ////////////////////////////////////////////////////////////////////////////


#define KiB(bytes) ((u64) bytes << 10)
#define MiB(bytes) ((u64) bytes << 20)
#define GiB(bytes) ((u64) bytes << 30)

class Arena
{
public:
  u64 size = 0;
  byte *memory = nullptr;
  byte *allocated = nullptr;
  byte *committed = nullptr;
  bool decommit_on_clear = false;
  u8 id = 0;

  Arena() {}
  Arena(u64 size);
  ~Arena();
  
  byte *push(u64 size, u32 align);
  void pop(u64 size);
  void clear();
  void release();
};

#define arena_push(arena, T, count) (T *) arena->push(size_of(T) * count, align_of(T))
#define arena_push_slc(arena, T, count) Slice<T>((T *) arena->push(size_of(T) * count, align_of(T)), count)

byte *align_ptr(byte *ptr, u32 align);

Arena::Arena(u64 size)
{
  this->memory = new byte[size];
  memset(this->memory, 0, size);
  
  this->allocated = this->memory;
  this->committed = this->memory;
  this->size = size;
  this->decommit_on_clear = true;
}

Arena::~Arena()
{
  this->release();
}

void Arena::release()
{
  delete[] this->memory;

  this->memory = nullptr;
  this->allocated = nullptr;
  this->size = 0;
}

[[nodiscard]]
byte *Arena::push(u64 size, u32 align)
{
  byte *ptr = align_ptr(this->allocated, align);
  this->allocated = ptr + size;
  return ptr;
}

void Arena::pop(u64 size)
{
  this->allocated -= size;
  
  // - Zero the memory ---
  u64 start_idx = (u64) (this->allocated - this->memory) - 1;
  u64 end_idx = start_idx + size;
  for (u64 i = start_idx; i < end_idx; i++)
  {
    this->allocated[i] = 0;
  }
}

void Arena::clear()
{
  for (u64 i = 0; i < (u64) (this->allocated - this->memory); i++)
  {
    this->memory[i] = 0;
  }

  this->allocated = this->memory;
}

byte *align_ptr(byte *ptr, u32 align)
{
	u64 result = (u64) ptr;
  u64 remainder = result % align;
  if (remainder != 0)
  {
    result += align - remainder;
  }

	return (byte *) result;
}


// Base_Slice ////////////////////////////////////////////////////////////////////////////


template <typename T>
class Slice
{
public:
  T *data = nullptr;
  u64 len = 0;

  Slice() {}

  Slice(cstr s)
  {
    this->from_ptr(s, strlen(s));
  }

  Slice(T *data, u64 count)
  {
    this->from_ptr(data, count);
  }

  Slice(T *data, u64 start, u64 end)
  {
    this->from_ptr(data, start, end);
  }

  inline 
  T &operator[](u64 index) const
  {
    assert(index >= 0);
    assert(index < this->len);

    return this->data[index];
  }

  inline 
  void operator=(const Slice<T> &other)
  {
    this->data = other.data;
    this->len = other.len;
  }

  inline
  bool operator==(const Slice<T> &other) const
  {
    if (this->len != other.len) return false;

    for (i32 i = 0; i < this->len; i++)
    {
      if (this->data[i] != other.data[i]) return false;
    }

    return true;
  }

  void from_ptr(T *data, u64 count=1)
  {
    assert(count >= 0);

    this->data = data;
    this->len = count;
  }

  void from_ptr(T *data, u64 start, u64 end)
  {
    assert(start < end);
    assert(start >= 0);

    this->data = &data[start];
    this->len = end - start;
  }

  Slice<T> slice(u64 start, u64 end) const
  {
    assert(start < end);
    assert(start >= 0);
    assert(end <= this->len);

    Slice<T> result;
    result.data = &this->data[start];
    result.len = end - start;

    return result;
  }

  cstr clone_to_cstr(Arena *arena) const
  {
    char *result = arena_push(arena, char, this->len+1);
    memcpy(result, this->data, this->len);
    result[this->len] = 0;
    return (cstr) result;
  }

  void print(bool new_line) const
  {
    for (u64 i = 0; i < this->len; i++)
    {
      printf("%c", this->data[i]);
    }

    if (new_line)
    {
      printf("\n");
    }
  }
};


// Base_Dynamic_Array ////////////////////////////////////////////////////////////////////


#ifndef DYNMAIC_ARRAY_GROWTH_RATE
#define DYNAMIC_ARRAY_GROWTH_RATE 2
#endif

template <typename T>
class Dynamic_Array
{
public:
  T *data = nullptr;
  u64 len = 0;
  u64 cap = 0;
  Arena *arena = nullptr;

  Dynamic_Array(Arena *arena)
  {
    this->arena = arena;
  }

  inline 
  T &operator[](u64 idx) const
  {
    assert(idx >= 0 && idx < this->len);
    return this->data[idx];
  }

  inline 
  void operator=(const Slice<T> &other)
  {
    this->data = other.data;
    this->len = other.len;
    this->cap = other.cap;
  }

  inline
  Slice<T> slice(u64 start, u64 end) const
  {
    return Slice<T>(this->data, start, end);
  }

  inline
  Slice<T> to_slice() const
  {
    return Slice<T>(this->data, this->len);
  }

  void append(T elem)
  {
    if (this->len >= this->cap)
    {
      u64 new_cap = (u64) ((f32) this->cap * DYNAMIC_ARRAY_GROWTH_RATE + 1);
      this->resize(new_cap);
    }

    this->data[this->len] = elem;
    this->len += 1;
  }

  void resize(u64 count)
  {
    if (count > this->cap)
    {
      T *new_data = arena_push(this->arena, T, count);
      memcpy((byte *) new_data, this->data, this->len * size_of(T));
      this->data = new_data;
      this->cap = count;
    }
  }

  void clear()
  {
    memset(this->data, 0, this->cap * size_of(T)); 
  }
};


// Base_String ///////////////////////////////////////////////////////////////////////////


#define S(lit) ((String){.data = (char *) lit, .len = size_of(lit) - 1})

class String
{
public:
  char *data = nullptr;
  u64 len = 0;

  static bool equals(String s1, String s2);
  static String copy(String s, Arena *arena);
  static String copy_into(String src, String *dest);
  static String copy_cstr(cstr s, u64 len, Arena *arena);
  static String concat(String s1, String s2, Arena *arena);

  String clone(Arena *arena) const;
  String clone_from_cstr(cstr s, Arena *arena) const;
  cstr clone_to_cstr(Arena *arena) const;
  String slice(u64 start, u64 end) const;
  Slice<char> to_slice() const;

  inline const 
  char &operator[](u64 index) const
  {
    assert(index >= 0 && index < this->len);

    return this->data[index];
  }

  inline const 
  void operator=(String other)
  {
    this->data = other.data;
    this->len = other.len;
  }

  inline const 
  bool operator==(String other)
  {
    return equals(*this, other);
  }
};

inline
u64 cstr_len(cstr s)
{
  u64 len = 0;
  for (; s[len]; len++);
  return len;
}

void copy_cstr_into_str(String *dest, cstr src)
{
  u64 len = cstr_len(src);
  for (u64 i = 0; i < len; i++)
  {
    dest->data[i] = src[i];
  }
  
  dest->len = len;
}

String str_from_cstr(cstr src)
{
  String result;

  u64 len = cstr_len(src);
  for (u64 i = 0; i < len; i++)
  {
    result.data[i] = src[i];
  }
  
  result.len = len;

  return result;
}

String str_from_slice(Slice<byte> slc)
{
  return {(char *) slc.data, slc.len};
}

String str_from_slice(Slice<char> slc)
{
  return {slc.data, slc.len};
}

String alloc_str(u64 len, Arena *arena)
{
  String result;
  result.data = arena_push(arena, char, len);
  result.len = len;

  for (u64 i = 0; i < len; i++)
  {
    result.data[i] = '\0';
  }

  return result;
}

String String::clone(Arena *arena) const
{
  return String::copy(*this, arena);
}

String String::clone_from_cstr(cstr cstr, Arena *arena) const
{
  return String::copy_cstr(cstr, cstr_len(cstr), arena);
}

cstr String::clone_to_cstr(Arena *arena) const
{
  char *result = arena_push(arena, char, this->len+1);
  memcpy(result, this->data, this->len);
  result[this->len] = '\0';
  return (cstr) result;
}

bool String::equals(String s1, String s2)
{
  if (s1.len != s2.len) return false;

  bool result = true;

  for (u64 i = 0; i < s1.len; i++)
  {
    if (s1.data[i] != s2.data[i])
    {
      result = false;
      break;
    }
  }

  return result;
}

String String::copy(String str, Arena *arena)
{
  String result;
  result.data = arena_push(arena, char, str.len+1);

  for (u64 i = 0; i < str.len; i++)
  {
    result.data[i] = str.data[i];
  }
  
  result.len = str.len;
  result.data[result.len] = '\0';

  return result;
}

String String::copy_into(String src, String *dest)
{
  for (u64 i = 0; i < src.len; i++)
  {
    dest->data[i] = src.data[i];
  }
  
  dest->len = src.len;

  return *dest;
}

String String::copy_cstr(cstr s, u64 len, Arena *arena)
{
  String result;
  result.data = arena_push(arena, char, len+1);

  for (u64 i = 0; i < len; i++)
  {
    result.data[i] = s[i];
  }

  result.len = len;
  result.data[result.len] = '\0';

  return result;
}

String String::concat(String s1, String s2, Arena *arena)
{
  String result = alloc_str(s1.len + s2.len + 1, arena);
  result.len -= 1;

  for (u64 i = 0; i < s1.len; i++)
  {
    result.data[i] = s1.data[i];
  }

  for (u64 i = 0; i < s2.len; i++)
  {
    result.data[i+s1.len] = s2.data[i];
  }
  
  result.data[result.len] = '\0';

  return result;
}

String String::slice(u64 start, u64 end) const
{
  assert(start < end);
  assert(start >= 0);
  assert(end > 0);
  assert(end <= this->len);

  String result = {0};
  result.data = this->data + start;
  result.len = static_cast<u64>(end - start);

  return result;
}

Slice<char> String::to_slice() const
{
  return Slice<char>((char *) this->data, this->len);
}

class String_Builder
{
public:
  Dynamic_Array<char> buffer;

  String_Builder(Arena *arena) : buffer(arena) {};

  void resize(u64 count)
  {
    this->buffer.resize(count);
  }

  void write_char(char chr)
  {
    this->buffer.append(chr);
  }

  void write_string(String str)
  {
    for (u64 i = 0; i < str.len; i++)
    {
      this->buffer.append(str[i]);
    }
  }
  
  String to_string() const
  {
    return str_from_slice(this->buffer.to_slice());
  }
};


// Main ///////////////////////////////////////////////////////////////////////////////////


enum Opcode_Name
{
  Opcode_NIL,

  Opcode_BEQ,
  Opcode_BNE,
  Opcode_BLT,
  Opcode_SW,

  Opcode_ADD,
  Opcode_SUB,
  Opcode_AND,
  Opcode_OR,

  Opcode_ADDI,
  Opcode_ANDI,
  Opcode_ORI,
  Opcode_SLL,
  Opcode_SRA,
  Opcode_LW,

  Opcode_JAL,
  Opcode_BREAK,

  Opcode_COUNT,
};

struct Instruction
{
  String bytecode_str;
  Opcode_Name opcode_name;
  u32 bytecode;
  u32 opcode;
  u32 rd;
  u32 rs1;
  u32 rs2;
  i32 imm;
  u32 fn3;
  u32 fn7;
};

Opcode_Name opcode_table[128];
String opcode_name_table[Opcode_COUNT];
String reg_name_table[32];

void init_global_data()
{
  // - Opcode table ---
  {
    opcode_table[0b0000000] = Opcode_BEQ;
    opcode_table[0b0000100] = Opcode_BNE;
    opcode_table[0b0001000] = Opcode_BLT;
    opcode_table[0b0001100] = Opcode_SW;

    opcode_table[0b0000001] = Opcode_ADD;
    opcode_table[0b0000101] = Opcode_SUB;
    opcode_table[0b0001001] = Opcode_AND;
    opcode_table[0b0001101] = Opcode_OR;

    opcode_table[0b0000010] = Opcode_ADDI;
    opcode_table[0b0000110] = Opcode_ANDI;
    opcode_table[0b0001010] = Opcode_ORI;
    opcode_table[0b0001110] = Opcode_SLL;
    opcode_table[0b0010010] = Opcode_SRA;
    opcode_table[0b0010110] = Opcode_LW;
    
    opcode_table[0b0000011] = Opcode_JAL;
    opcode_table[0b1111111] = Opcode_BREAK;
  }

  // - Opcode name table ---
  {
    opcode_name_table[Opcode_BEQ] = S("beq");
    opcode_name_table[Opcode_BNE] = S("bne");
    opcode_name_table[Opcode_BLT] = S("blt");
    opcode_name_table[Opcode_SW]  = S("sw");

    opcode_name_table[Opcode_ADD] = S("add");
    opcode_name_table[Opcode_SUB] = S("sub");
    opcode_name_table[Opcode_AND] = S("and");
    opcode_name_table[Opcode_OR]  = S("or");

    opcode_name_table[Opcode_ADDI] = S("addi");
    opcode_name_table[Opcode_ANDI] = S("andi");
    opcode_name_table[Opcode_ORI]  = S("ori");
    opcode_name_table[Opcode_SLL]  = S("sll");
    opcode_name_table[Opcode_SRA]  = S("sra");
    opcode_name_table[Opcode_LW]   = S("lw");

    opcode_name_table[Opcode_JAL]   = S("jal");
    opcode_name_table[Opcode_BREAK] = S("break");
  }

  // - Register name table ---
  {
    reg_name_table[0]  = S("x0");
    reg_name_table[1]  = S("x1");
    reg_name_table[2]  = S("x2");
    reg_name_table[3]  = S("x3");
    reg_name_table[4]  = S("x4");
    reg_name_table[5]  = S("x5");
    reg_name_table[6]  = S("x6");
    reg_name_table[7]  = S("x7");
    reg_name_table[8]  = S("x8");
    reg_name_table[9]  = S("x9");
    reg_name_table[10] = S("x10");
    reg_name_table[11] = S("x11");
    reg_name_table[12] = S("x12");
    reg_name_table[13] = S("x13");
    reg_name_table[14] = S("x14");
    reg_name_table[15] = S("x15");
    reg_name_table[16] = S("x16");
    reg_name_table[17] = S("x17");
    reg_name_table[18] = S("x18");
    reg_name_table[19] = S("x19");
    reg_name_table[20] = S("x20");
    reg_name_table[21] = S("x21");
    reg_name_table[22] = S("x22");
    reg_name_table[23] = S("x23");
    reg_name_table[24] = S("x24");
    reg_name_table[25] = S("x25");
    reg_name_table[26] = S("x26");
    reg_name_table[27] = S("x27");
    reg_name_table[28] = S("x28");
    reg_name_table[29] = S("x29");
    reg_name_table[30] = S("x30");
    reg_name_table[31] = S("x31");
  }
}

Instruction instruction_from_bytecode(u32 bytecode)
{
  Instruction instruction = {};
  instruction.bytecode = bytecode;
  instruction.opcode = bytecode & 0b1111111;
  instruction.opcode_name = opcode_table[instruction.opcode];
  
  u32 format = bytecode & 0b11;
  switch (format)
  {
  case 0b00:
    instruction.imm |= (      bytecode & (      (i32) 0b11111   << 7))  >> 7;
    instruction.fn3 |= (      bytecode & (      (u32) 0b111     << 12)) >> 12;
    instruction.rs1 |= (      bytecode & (      (u32) 0b11111   << 15)) >> 15;
    instruction.rs2 |= (      bytecode & (      (u32) 0b11111   << 20)) >> 20;
    instruction.imm |= ((i32) bytecode & (i32) ((u32) 0b1111111 << 25)) >> 25;
    break;

  case 0b01:
    instruction.rd  |= (bytecode & ((u32) 0b11111   << 7))  >> 7;
    instruction.fn3 |= (bytecode & ((u32) 0b111     << 12)) >> 12;
    instruction.rs1 |= (bytecode & ((u32) 0b11111   << 15)) >> 15;
    instruction.rs2 |= (bytecode & ((u32) 0b11111   << 20)) >> 20;
    instruction.fn7 |= (bytecode & ((u32) 0b1111111 << 25)) >> 25;
    break;

  case 0b10:
    instruction.rd  |= (      bytecode & (      (u32)       0b11111  << 7))  >> 7;
    instruction.fn3 |= (      bytecode & (      (u32)       0b111    << 12)) >> 12;
    instruction.rs1 |= (      bytecode & (      (u32)       0b11111  << 15)) >> 15;
    instruction.imm |= ((i32) bytecode & (i32) ((u32) 0b111111111111 << 20)) >> 20;
    break;

  case 0b11:
    instruction.rd  |= (      bytecode & (      (u32) 0b11111                 << 7))  >> 7;
    instruction.imm |= ((i32) bytecode & (i32) ((u32) 0b111111111111111111111 << 12)) >> 12;
    break;
  }

  return instruction;
}

i32 bytecode_from_string(String str)
{
  i32 result = 0;

  for (u64 i = 0; i < str.len; i++)
  {
    char c = str[str.len-1-i];
    if (c == '1')
    {
      result += 1 << i;
    }
    else if (c != '0')
    {
      result = -1;
      break;
    }
  }

  return result;
}

void disassemble(Slice<Instruction> instructions, Arena *temp)
{
  bool hit_break = false;

  for (u64 i = 0; i < instructions.len; i++)
  {
    Instruction &instr = instructions[i];
    String_Builder builder = String_Builder(temp);
    u32 address = 256 + ((u32) i * 4);
    
    if (hit_break)
    {
      char bytecode_buf[64] = {};
      i32 bytecode_len = snprintf(bytecode_buf, 64, "%i", instr.bytecode);
      builder.write_string(String{bytecode_buf, (u64) bytecode_len});
    }
    else
    {
      String opcode_str = opcode_name_table[instr.opcode_name];
      builder.write_string(opcode_str);
      builder.write_string(S(" "));

      char fmt_buf[128] = {};
      i32 fmt_len = 0;

      u8 format = instr.bytecode & 0b11;
      switch (format)
      {
      case 0b00:
        builder.write_string(reg_name_table[instr.rd]);
        builder.write_string(S(", "));
        builder.write_string(reg_name_table[instr.rs1]);
        builder.write_string(S(", "));
        builder.write_char('#');
        fmt_buf[64] = {};
        fmt_len = snprintf(fmt_buf, 64, "%i", instr.imm);
        builder.write_string(String{fmt_buf, (u64) fmt_len});
        break;

      case 0b01:
        builder.write_string(reg_name_table[instr.rd]);
        builder.write_string(S(", "));
        builder.write_string(reg_name_table[instr.rs1]);
        builder.write_string(S(", "));
        builder.write_string(reg_name_table[instr.rs2]);
        builder.write_string(S(" "));
        break;

      case 0b10:
        builder.write_string(reg_name_table[instr.rd]);
        builder.write_string(S(", "));
        builder.write_string(reg_name_table[instr.rs1]);
        builder.write_string(S(", "));
        builder.write_char('#');
        fmt_buf[64] = {};
        fmt_len = snprintf(fmt_buf, 64, "%i", instr.imm);
        builder.write_string(String{fmt_buf, (u64) fmt_len});
        break;

      case 0b11:
        builder.write_string(reg_name_table[instr.rd]);
        builder.write_string(S(", "));
        builder.write_char('#');
        fmt_buf[64] = {};
        fmt_len = snprintf(fmt_buf, 64, "%i", instr.imm);
        builder.write_string(String{fmt_buf, (u64) fmt_len});
        break;
        
        break;
      }
    }

    if (instr.opcode_name == Opcode_BREAK)
    {
      hit_break = true;
    }

    printf("%s      %i      %s\n", 
           instr.bytecode_str.clone_to_cstr(temp), 
           address, 
           builder.to_string().clone_to_cstr(temp));
    temp->clear();
  }

  temp->clear();
}

void simulate(Slice<Instruction> instructions, Arena *temp)
{
  static
  i32 registers[32] = {};

  static
  std::map<i32, i32> memory = {};

  for (i32 pc_idx = 0; pc_idx < instructions.len;)
  {
    Instruction &instr = instructions[(u32) pc_idx];
    String_Builder builder = String_Builder(temp);
    char fmt_buf[128] = {};
    i32 fmt_len = 0;
    i32 mem_addr = 0;
    i32 jump_off = 1;
    
    switch (instr.opcode_name)
    {
      default:
        break;

      case Opcode_BEQ:
        if (registers[instr.rs1] == registers[instr.rs2])
        {
          // jump_off = instr.imm;
        }

        break;

      case Opcode_BNE:
        if (registers[instr.rs1] != registers[instr.rs2])
        {

        }

        break;

      case Opcode_BLT:
        if (registers[instr.rs1] < registers[instr.rs2])
        {

        }
        
        break;

      case Opcode_SW:
        mem_addr = registers[instr.rs2] + instr.imm;
        memory[mem_addr] = registers[instr.rs1];
        break;

      case Opcode_ADD:
        registers[instr.rd] = registers[instr.rs1] + registers[instr.rs2];
        break;

      case Opcode_SUB:
        registers[instr.rd] = registers[instr.rs1] - registers[instr.rs2];
        break;

      case Opcode_AND:
        registers[instr.rd] = registers[instr.rs1] & registers[instr.rs2];
        break;

      case Opcode_OR:
        registers[instr.rd] = registers[instr.rs1] | registers[instr.rs2];
        break;

      case Opcode_ADDI:
        registers[instr.rd] = registers[instr.rs1] + instr.imm;
        break;

      case Opcode_ANDI:
        registers[instr.rd] = registers[instr.rs1] & instr.imm;
        break;

      case Opcode_ORI:
        registers[instr.rd] = registers[instr.rs1] | instr.imm;
        break;

      case Opcode_SLL:
        registers[instr.rd] = registers[instr.rs1] << instr.imm;
        break;

      case Opcode_SRA:
        registers[instr.rd] = registers[instr.rs1] >> instr.imm;
        break;

      case Opcode_LW:
        mem_addr = registers[instr.rs2] + instr.imm;
        memory[mem_addr] = registers[instr.rs1];
        break;

      case Opcode_JAL:
        break;

      case Opcode_BREAK:
        break;
    }

    pc_idx += jump_off;

    // - Print ---
    {
      builder.write_string(S("--------------------\n"));
      builder.write_string(S("Cycle "));
    }

    
    printf("%s\n", builder.to_string().clone_to_cstr(temp));
    temp->clear();
  }
}

i32 main(i32 argc, char *argv[])
{
  Arena perm_arena = Arena(GiB(1));
  Arena temp_arena = Arena(GiB(1));

  if (argc < 2) return 0;

  init_global_data();

  cstr input_path_cstr = argv[1];
  i32 input_path_fd = open(input_path_cstr, O_RDONLY);
  
  byte contents_buf[1024] = {};
  i64 contents_size = read(input_path_fd, contents_buf, 1024);
  if (contents_size < 1)
  {
    printf("Error reading file!");
    return 1;
  }

  contents_buf[contents_size] = '\n';
  contents_size += 1;

  Slice<byte> contents = Slice(contents_buf, (u64) contents_size);

  // - Decode program ---
  Dynamic_Array<Instruction> instructions(&perm_arena);
  for (u32 i = 0; i < contents_size;)
  {
    if (contents[i] == '\n')
    {
      String bytecode_str = str_from_slice(contents.slice(i-32, i));
      u32 bytecode = (u32) bytecode_from_string(bytecode_str);
      Instruction instruction = instruction_from_bytecode(bytecode);
      instruction.bytecode_str = bytecode_str;
      instructions.append(instruction);
      i += 32;
    }
    else
    {
      i += 1;
    }
  }

  // - Disassembl program ---
  // disassemble(instructions.to_slice(), &temp_arena);
  // printf("\n\n");

  // - Simulate program ---
  simulate(instructions.to_slice(), &temp_arena);

  return 0;
}
