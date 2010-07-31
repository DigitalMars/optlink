
// MVLNAME - Copyright (c) SLR Systems 1994

ubyte* MOVE_ASCIZ_ECX_EAX(ubyte* ECX, ubyte* EAX)
{
	// ECX IS SOURCE, EAX IS DESTINATION
	size_t len = strlen(ECX);
	return memcpy(EAX, ECX, len + 1) + len;
}


ubyte* MOVE_ASCIZ_ESI_EDI(ubyte* ESI, ubyte* EDI)
{
	// ESI IS SOURCE, EDI IS DESTINATION
	return MOVE_ASCIZ_ECX_EAX(ESI, EDI);
}
