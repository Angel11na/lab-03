# M1 — адреси гостя це числа. 

Реалізовано `get16` та `set16` які зчитуючи та записують по 2 байти.

`set16 0 0x1234` дає байти `34 12`:
```
ember> set16 0 0x1234
ember> dump
0000  34 12 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |4...............................|
0020  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |................................|
```


# M2 — група 0x2_. 

Реалізовано рядки `0x20–0x2C` з **ISA.uk.md**: 
```cpp
//  --- DATA
const Byte OP_LOADI_A           = 0x0;  //  Load 8 bits to A
const Byte OP_LOADI_B           = 0x1;  //  Load 8 bits to B
const Byte OP_LOAD_A            = 0x2;  //  Load 8 bits to A from mem[addr16]
const Byte OP_LOAD_B            = 0x3;  //  Load 8 bits to B from mem[addr16]
const Byte OP_STORE_A           = 0x4;  //  Put A to mem[addr16]
const Byte OP_STORE_B           = 0x5;  //  Put B to mem[addr16]
const Byte OP_MOV_AB            = 0x6;  //  A = B
const Byte OP_MOV_BA            = 0x7;  //  B = A
const Byte OP_LOADH             = 0x8;  //  Load 16 bit to H
const Byte OP_LOAD_AH           = 0x9;  //  A = mem[H]
const Byte OP_STORE_HA          = 0xa;  //  mem[H] = A
const Byte OP_INC_H             = 0xb;  //  H +1
const Byte OP_DEC_H             = 0xc;  //  H -1
const Byte OP_H_LOW             = 0xd;  //  low byte from H
```
 
Перевірка операції `LOADI A`:
```
❯ ./build/ember
ember 0.1 - 4096 bytes of memory you can see. Type `help`.
ember> set 0 0x20
ember> set 1 0xab
ember> regs
PC = 0x0
H = 0x00  0000_0000_0000_0000
A = 0  0x0  00000000  '.'
B = 0  0x0  00000000  '.'
Z: 0
N: 0
C: 0
ember> step
ember> regs
PC = 0x2
H = 0x00  0000_0000_0000_0000
A = 171  0xab  10101011  '.'
B = 0  0x0  00000000  '.'
Z: 0
N: 0
C: 0
ember> 
```


# M3 — програма, яка користується даними. 

Покладіть на `0x800` байти повідомлення `(65 66 67 0 — ABC)`:
```
ember> set 0x800 65
ember> set 0x801 66
ember> set 0x802 67
ember> set 0x803 0
ember> dump
...
07e0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |................................|
0800  41 42 43 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |ABC.............................|
0820  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |................................|
...
```

## Без використання H
На `0x000` — програму, яка завантажує з `0x800` у `A`, робить `OUT` і зупиняється:
```
ember> set 0 0x22   # LOAD A from Addr
ember> set 1 0x00   # Addr.low
ember> set 2 0x08   # Addr.high
ember> set 3 0x02   # OUT
ember> set 4 0x00   # HALT
```
 
Запустіть через `run` (крокувати до `HALT`). Вставте рядок термінала, де надрукувалась літера, і `dump` обох регіонів:
```
ember> run
A
ember> dump
0000  22 00 08 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |"...............................|
0020  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |................................|
...
07e0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |................................|
0800  41 42 43 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |ABC.............................|
0820  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |................................|
...
```

## Те саме, але через `H`: 
- `LOADH H, 0x0800`
- `LOAD A, [H]`
- `OUT`
- `INCH`
- `LOAD A, [H]`
- `OUT`
- `HALT`


Послідовність команд для `ember`:
```
set 0x800 65
set 0x801 66
set 0x802 67
set 0x803 0
set 0 0x28
set 1 0x00
set 2 0x08
set 3 0x29
set 4 0x02
set 5 0x2b
set 6 0x29
set 7 0x02
set 8 0x00

```

Виведення:

```
ember 0.1 - 4096 bytes of memory you can see. Type `help`.
ember> set 0x800 65
set 0x801 66        # артефакт виведення в терміналі Linux через вставлення тексту
set 0x802 67
set 0x803 0
set 0 0x28
set 1 0x00
set 2 0x08
set 3 0x29
set 4 0x02
set 5 0x2b
set 6 0x29
set 7 0x02
set 8 0x00
                    # артефакт виведення в терміналі Linux через вставлення тексту
ember> ember> ember> ember> ember> ember> ember> ember> ember> ember> ember> ember> ember> run
A
B
ember> regs
PC = 0x9
H = 0x801  0000_1000_0000_0001
A = 66  0x42  01000010  'B'
B = 0  0x0  00000000  '.'
Z: 0
N: 0
C: 0
ember> dump
0000  28 00 08 29 02 2b 29 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |(..).+).........................|
0020  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |................................|
...
0800  41 42 43 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |ABC.............................|
...
```


# M4 — вказівник хоста проти адреси гостя. 

`CPU` зберігає пам'ять як `Memory* mem`, це вказівник хоста на всю коробку пам'яті як на об'єкт. Він указує на `struct Memory` цілком, а не на перший байт `data`, бо коробка - це не просто масив, а структура з полями, і працювати з нею треба як з єдиним об'єктом. Наприклад, можна додати ще полій, й об'єкт `CPU` матиме до них доступ.
Натомість `pc` і `H` - це 16-бітні числа, за допомогою яких можна переміщуватися по коробці. З ними можна рахувати (`H++`, `H + 0x100`), порівнювати їх із межею пам'яті, класти в комірки й діставати назад. `Byte*` хоста для гостя не означає нічого так як це 64-бітна адреса, щоразу інша при запуску, і вона фізично не влазить у 16-бітну комірку. Працювати з 16 бітами через `Byte*` можна, але це менш зручно, потребує додаткових зусиль та перевірок. Вказівник на повний об'єкт пам'яті універсальніший та зручніший у використанні.

Перевірка `data[MEM_SIZE] = 1` (вихід на одиницю за межу):
```
❯ ./build/ember
/home/angelll/Documents/progamming/lab-03/src/main.cpp:36:22: runtime error: index 4096 out of bounds for type 'unsigned char [4096]'
/home/angelll/Documents/progamming/lab-03/src/main.cpp:36:24: runtime error: store to address 0x7b4fd59f12a0 with insufficient space for an object of type 'Byte'
0x7b4fd59f12a0: note: pointer points here
 00 00 00 00  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  00 00 00 00
              ^ 
=================================================================
==9215==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7b4fd59f12a0 at pc 0x562a8e8d7c2a bp 0x7fff1ceddde0 sp 0x7fff1cedddd0
WRITE of size 1 at 0x7b4fd59f12a0 thread T0
    #0 0x562a8e8d7c29 in main /home/angelll/Documents/progamming/lab-03/src/main.cpp:36
    #1 0x7f4fd7627ccd  (/usr/lib/libc.so.6+0x27ccd) (BuildId: 78e50f499365f967d6249585154c0ade819bfba8)
    #2 0x7f4fd7627e0a in __libc_start_main (/usr/lib/libc.so.6+0x27e0a) (BuildId: 78e50f499365f967d6249585154c0ade819bfba8)
    #3 0x562a8e8d74b4 in _start (/home/angelll/Documents/progamming/lab-03/build/ember+0x394b4) (BuildId: 90637fab5f9d31a7ee5c8e41270183b2b9c32974)

Address 0x7b4fd59f12a0 is located in stack of thread T0 at offset 4768 in frame
    #0 0x562a8e8d781e in main /home/angelll/Documents/progamming/lab-03/src/main.cpp:30

  This frame has 5 object(s):
    [32, 56) 'cpu' (line 32)
    [96, 128) 'line' (line 43)
    [160, 192) 'cmd' (line 55)
    [224, 608) 'words' (line 54)
    [672, 4768) 'mem' (line 31) <== Memory access at offset 4768 overflows this variable
HINT: this may be a false positive if your program uses some custom stack unwind mechanism, swapcontext or vfork
      (longjmp and C++ exceptions *are* supported)
SUMMARY: AddressSanitizer: stack-buffer-overflow /home/angelll/Documents/progamming/lab-03/src/main.cpp:36 in main
Shadow bytes around the buggy address:
  0x7b4fd59f1000: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7b4fd59f1080: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7b4fd59f1100: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7b4fd59f1180: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7b4fd59f1200: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x7b4fd59f1280: 00 00 00 00[f3]f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3
  0x7b4fd59f1300: f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3
  0x7b4fd59f1380: f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3
  0x7b4fd59f1400: f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3
  0x7b4fd59f1480: f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3
  0x7b4fd59f1500: f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3 f3
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==9215==ABORTING
```
