#define MESSAGE "pure virtual method called\n"

void __pure_virtual ()
{
  write (2, MESSAGE, sizeof (MESSAGE) - 1);
  exit(3);
}
