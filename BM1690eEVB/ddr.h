#define DDR_SIZE_2R_256G	5 // x8 32Gb
#define DDR_SIZE_2R_128G_x16	4 // x16 32Gb
#define DDR_SIZE_2R_128G	3 // x8 16Gb
#define DDR_SIZE_2R_64G		2 // x16 16Gb
#define DDR_SIZE_1R		1 // x16 16Gb

void ddr_size_init(int mode);
int get_ddr_size();
