int main()
{
    int i;

    for(i = 0; i < 16384; i++)
        if(i < 16)
	    putchar(i);
	else
	    putchar(0xbe);
}
