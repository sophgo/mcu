static int ddr_size;

void ddr_size_init(int size)
{
	ddr_size = size;
}


int get_ddr_size()
{
	return ddr_size;
}
