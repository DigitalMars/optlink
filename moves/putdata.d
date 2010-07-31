// PUTDATA - Copyright (c) SLR Systems 1994


version(fg_segm)
{
IMP_OUT(EAX, ECX)
{
    put_data(EAX, ECX, IMP_DEVICE);
}
}

LST_OUT(EAX, ECX)
{
    // EAX IS POINTER
    // ECX IS BYTE COUNT

    version(fgh_mapthread)
    {
	// ERRORS AND WRITEMAP MAY BOTH WANT..
	CAPTURE	PUT_DATA_SEM
	put_data(EAX, ECX, MAP_DEVICE);
	RELEASE	PUT_DATA_SEM
    }
    else
    {
	put_data(EAX, ECX, MAP_DEVICE);
    }
}

private put_data(ps, uint count, MYO_STRUCT* pm)
{
    // PUT DATA TO ANY OUTPUT STRUCTURE POINTED TO BY BX
    // FLUSH IF BUFFER IS FULL

    // ps IS SOURCE
    // count IS BYTE COUNT
    // pm IS BUFFER STRUCTURE

    if (count && pm)
    {
	pm.MYO_BYTE_OFFSET = pm.MYO_BYTE_OFFSET + count;
	while (1)
	{
	    if (pm.MYO_COUNT >= count)
	    {	pm.MYO_COUNT -= count;
		break;
	    }

	    if (pm.MYO_COUNT)
	    {
		memcpy(pm.MYO_PTR, ps, pm.MYO_COUNT);
		pm.MYO_PTR += pm.MYO_COUNT;
		count -= pm.MYO_COUNT;
		pm.MYO_COUNT = 0;
	    }
	    pm.MYO_FLUSHBUF();
	}

	memcpy(pm.MYO_PTR, ps, count);
	pm.MYO_PTR += count;
    }
}

