// GTNXTU - Copyright (c) SLR Systems 1994


// Get next character from EBX in upper case
unsigned char GTNXTU(const unsigned char **EBX)
{
    unsigned char AL = *(*EBX)++;
    if (AL >= 'a' && AL <= 'z')
	AL -= 0x20;
    return AL;
}
