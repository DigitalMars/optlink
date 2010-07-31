
int _loutall_con(unsigned nbytes, char *ptr);

void DOT()
{
    _asm
    {
	pushad
    }
    _loutall_con(1, ".");
    _asm
    {
	popad
    }
}
