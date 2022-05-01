
struct Setting{
	// for setting
	int limit;			// max level 
	int type;			// graph type
	int pb;				// num. of pattern bit
	int a[2];			// range of floor
	int b[2];			// range of not encode bits
	int re_arange;		// 0 = normal, 1 = sorted, 2 = feature
	int arange_block;	// 0 = full
	int px_mode;		// 0 = normal, 1 = random, 2 = sorted, 3 = weighted and sort, 4 = count only 
	int iter_type;		// iteration type, 0 = none, 1 = fixed, 2 = random, 3 = shift, 4 = diffusion, 5 = count only and weighted
	int shift_block;	// shift block size
	int mid_type;		// 0 = average, 1 = middle
	bool fix_level_bit;	// 1 = used fixed bits in iteration
	int split_type;		// 2 or 3
	int data_split;
};

struct Task{
	// about data
	char* name;		// name of this task
	int num;		// num of this test data
	int *datas;		// datas to be handle
	int *sdata1;
	int *sdata2;
	int total;			// total bits in this file
	int numincube;		// # of data in one cube

	// temp
	int f;			// start not encode
	int n;			// num. of not encode bit
	int iteration;	// the iteration of reencode
	int mid;		// the median level of this time encode 
	int mean;		// the average level of this time encode
	int encodetimes;// the total encode time of this turn
	int nectimes;	// the total not encode time of this turn
	int sd;			// the Standard Deviation of levels in this time of encode
	int r4[2];		// range of p4
	int r3[2];		// range of p3
	int lmax;		// max level meet of this data
	int temp_level;	// temp. max level
	
	// record
	int after;			// of #bits after this time compressed 
	int best[5];		
		// best case of [graph type] [f] [n] [iteration] [#bits after compressed] (for every file)
	int apx[16];
	int recoedrange[4];
	int *detail_recoed;
	double start, end;	// time record
};
