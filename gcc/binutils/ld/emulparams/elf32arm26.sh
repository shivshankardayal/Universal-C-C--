TEMPLATE_NAME=elf32
GENERATE_SHLIB_SCRIPT=yes
SCRIPT_NAME=elf
OUTPUT_FORMAT="elf32-arm"
# this needs to be high enough so that we can load ld.so below it,
# yet low enough to stay away from the mmap area at 0x01100000
# also, it is small enough so that relocs which are pointing
# at absolute 0 will still be fixed up
# These values give us about 0.5MB for ld.so, 16.5MB for user 
# programs, and 15MB for mmap which seems a reasonable compromise.
TARGET_PAGE_SIZE=0x8000
TEXT_START_ADDR=0x00080000
MAXPAGESIZE=0x8000
ARCH=arm
