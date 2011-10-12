TEMPLATE_NAME=elf32
GENERATE_SHLIB_SCRIPT=yes
SCRIPT_NAME=elf
OUTPUT_FORMAT="elf32-arm"
# this needs to be high enough so that we can load ld.so below it,
# yet low enough to stay away from the mmap area 0x40000000
# also, it is small enough so that relocs which are pointing
# at absolute 0 will still be fixed up
TEXT_START_ADDR=0x02000000
MAXPAGESIZE=0x8000
ARCH=arm
