

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <vector>

#include "struct.c"
#include "translate.c"
#include "io.c"
#include "pxs.c"
#include "getcode.c"

using namespace std;

// compression data
vector<int> outcomes;	// record num of bits before and after compression

vector<string> cws;		// codeword

// compression record
int recordpx[9][16];
int recordr[9][4];

//============================================= compression =========================================================
void getcodeword(Task* ptrt, Setting *ptrs, bool split, bool dorecord)
{
	int *pxrecord_position;
	
	for(int i = 0; i < levels.size(); i++)
	{
		// not encode
		if(levels[i] == 0){cws.push_back("0" + GetBin(pxs[i], ptrt->n));}
		// encode
		else
		{
			if(ptrs->fix_level_bit && ptrt->iteration != 0)		
			{	// for iteration fixed level bit
				if(levels[i]+ptrt->f > ptrt->r4[0] && levels[i]+ptrt->f < ptrt->r4[1]){	cws.push_back(GetBin(levels[i], ceil(log(ptrt->temp_level)/log(2))) + tail(pxs[i], 4, levels[i]));	}
				else{	cws.push_back(GetBin(levels[i], ceil(log(ptrt->temp_level)/log(2))) + tail(pxs[i], 2, levels[i]));	}
			}
			else	
			{	// original compression
				if(!split){	 cws.push_back(prefix(levels[i]) + tail(pxs[i], 4, levels[i]));}
				// iteration compression
				else
				{
					if(levels[i] > ptrt->r4[0] && levels[i] <= ptrt->r4[1]){	cws.push_back(prefix(levels[i]) + tail(pxs[i], 4, levels[i]));	}
					//else if(levels[i] + ptrt->f > ptrt->r3[0] && levels[i] + ptrt->f < ptrt->r3[1]){cws.push_back(prefix(levels[i]) + tail(pxs[i], 3, levels[i]));	}
					else{	cws.push_back(prefix(levels[i]) + tail(pxs[i], ptrs->split_type, levels[i]));	}
				}
			}
	
			//for record
			if(dorecord)
			{
				// for sort
				pxrecord_position = ptrt->detail_recoed + pxs[i] * ptrs->limit + levels[i] + ptrt->f;
				switch(ptrs->px_mode)
				{
					case 2:	pxrecord[pxs[i]] = pxrecord[pxs[i]] + 1;							break;	// sort all
					case 3:	pxrecord[pxs[i]] = pxrecord[pxs[i]] + levels[i];					break;	// weighted sort all
					case 4:	*(pxrecord_position)=*(pxrecord_position) + 1;						break;	// sort only
					case 5:	*(pxrecord_position)=*(pxrecord_position) + levels[i] + 1;	break;	// weighted sort all
					default: cout << "Error px mode!" << endl;	break;		
				}
			}
		}
		if(dorecord){	levelrecord[levels[i] + ptrt->n]++;	}
		if(dorecord){	if(ptrt->lmax < levels[i] + ptrt->n){ptrt->lmax = levels[i] + ptrt->n;	}
		}
	}
}

void compression(Task* ptrt, Setting *ptrs, bool split)
{
	//initial variable
	int head = 0, level, numofbits, ref = 4;
	ptrt->nectimes = 0;
	
	while(head < ptrt->total)			// loop for one cube
	{
		level = ptrt->temp_level;
		
		while(level > ptrt->f - 1)				// keep testing until match
		{	
			// not encode
			if(level == ptrt->f)
			{
				numofbits = ptrt->n;
				ptrt->nectimes++;
				if(ptrt->total - head > ptrt->n - 1){	pxs.push_back(GetDec(ptrt->datas + head, ptrt->n - 1));	break;}
				else{pxs.push_back(GetDec(ptrt->datas + head, ptrt->total - head - 1));	break;	}
			}
			
			numofbits = graphnum(level, ptrs->type);
			
			// remaining bit not enough for this level's test
			if(ptrt->total - head < numofbits){level--; continue;	}
			
			// encode
			if(!split){if(graph(level, ptrs->type, ptrt->datas + head, numofbits, ptrs->pb)){ptrt->encodetimes++;	break;	}}
			else
			{
				if(level - ptrt->f > ptrt->r4[0] && level - ptrt->f <= ptrt->r4[1]){	if(graph(level, ptrs->type, ptrt->datas + head, numofbits, 4)){break;}}
				//else if(level > ptrt->r3[0] && level < ptrt->r3[1] ){	if(graph(level, ptrt->type, ptrt->datas + head, numofbits, 3)){break;}}
				else{	if(graph(level, ptrs->type, ptrt->datas + head, numofbits, ptrs->split_type)){break;}}
			}
			level = level - 1;
		}
		levels.push_back(level - ptrt->f);
		//ptrt->mean = ptrt->mean + level - ptrt->f;
		head = head + numofbits;
		//if(ptrt->mode == 0){ cout << "	" << levels[levels.size() - 1] << "	" << pxs[pxs.size() - 1] << "	" << numofbits << endl;	}
	}
}

void analysis(Task *ptrt)
{
	float SD;
	int middle = 0;

	// Average
	for(int k = 0; k < levels.size(); k++){	ptrt->mean = ptrt->mean + levels[k];}
	ptrt->mean = ptrt->mean / float(ptrt->encodetimes);
	
	// Standard Deviation
	for( int j = 0; j < levels.size(); j++){if(levels[j] > 0){SD = SD + pow( levels[j] - ptrt->mean, 2);}	}
	SD = SD / ptrt->encodetimes;
	ptrt->sd = sqrt(double(SD));
		
	// Median
	for(int n = 0; n < levelrecord.size(); n++){	middle = middle + levelrecord[n];}
	middle = (middle / 2) - ptrt->nectimes;

	for(int m = 0; m < levelrecord.size(); m++)
	{
		middle = middle - levelrecord[m];
		if(middle < 0){ptrt->mid = m;	break;} 
	}
	
	levels.clear();	pxs.clear();	vector <int>().swap(levels);	vector <int>().swap(pxs);
}

int countafter()
{
	int temp = 0;
	for(int k = 0; k < cws.size(); k++){	temp = temp + cws[k].size();	}
	
	// clear for next 
	cws.clear();	vector<string>().swap(cws);
	
	return temp;
}

void output_vec(Setting *ptrs, Task* ptrt)
{
	fstream fp;
	char *output = ptrt->name;
	*(output+3) = 'O';	*(output+4) = 'u';	*(output+5) = 't';
	
	fp.open(output, ios::out);
	if(!fp.is_open()){ cout << "Unable open " << output << endl;}
	else
	{
		cout << "Output .vec file..." << endl; 
		
		dataArang(ptrt, ptrs);
		ptrt->f = ptrt->best[1];
		ptrt->n = ptrt->best[2];
		ptrt->iteration = ptrt->best[3];
		
		for(int i = 0; i < 16; i++){	pxr[i] = ptrt->apx[i];		}
		
		cout << ptrt->num+1 << "	| f: " << ptrt->f << " | n: " << ptrt->n << endl;
		compression(ptrt, ptrs, true); 		//cout << "Compression OK!" << endl;
		getcodeword(ptrt, ptrs, true, false);	//cout << "Encode OK!" << endl;
		
		for(int j = 0; j < cws.size(); j++){	fp << cws[j] << endl;}
		
		cws.clear();	vector<string>().swap(cws);
	}
	fp.close();
	
	cout << "Output .vec file end!" << endl;
}

//================================================ output =============================================================
void reArangePx(int* p, int p4wall, int maxlevel)
{
	int pxbuf[16] = {1};
	int max = 0, temp = 0;
	
	// caculate
	for(int i = 0; i < 16; i++)
	{
		for(int j = p4wall; j < maxlevel; j++){	pxbuf[i] = pxbuf[i] + *(p+i*maxlevel+j);}
		if(max < pxbuf[i]){	max = pxbuf[i];}
	}
	
	// sort
	while(max > 0)
	{
		for(int k = 0; k < 16; k++)
		{
			if(max == pxbuf[k])
			{
				pxbuf[k] = 0;
				pxr[temp] = k;
				temp++;
			}
		}
		max--;
	}
}

void outcome()
{
	float CR, ave;
	
	cout << endl << endl;
	for(int i = 0; i < 6; i++)
	{
		CR = ((outcomes[6 * i] - outcomes[6 * i + 1] ) ) * 100 / float(outcomes[6 * i]);
		cout << "	-------------------------------------------------------------------------" << endl;
		cout << "	| G:" << outcomes[6*i+5] << " |" << outcomes[6*i+4] << " / " << outcomes[6*i+3] << " / " << outcomes[6*i+2];
		cout << "	| " << recordr[i][1] << ", " << recordr[i][2];  
		
		cout << "	| 		CR: " << setprecision(4) << CR << "%	|" << endl;
		cout << "	-------------------------------------------------------------------------" << endl;
		cout << "	|	";
		for(int r = 0; r < 16 ; r++){	cout << recordpx[i][r] << ", ";} 
		cout << "		|" << endl;
		
		ave = ave + CR;
	}
	cout << "	-------------------------------------------------------------------------" << endl;
	ave = ave / 6;
	cout << endl << "	Average: " << setprecision(4) << ave << endl << endl;

	outcomes.clear();	vector <int>().swap(outcomes);
}

void outcomecompair(Task *ptrt, Setting *ptrs)
{
	if(ptrt->after < ptrt->best[4])
	{
		ptrt->best[4] = ptrt->after;	ptrt->best[3] = ptrt->iteration;
		ptrt->best[2] = ptrt->n; 		ptrt->best[1] = ptrt->f;
		ptrt->best[0] = ptrs->type;
		ptrt->recoedrange[0] = ptrt->r3[0];
		ptrt->recoedrange[1] = ptrt->r4[0];
		ptrt->recoedrange[2] = ptrt->r4[1];
		ptrt->recoedrange[3] = ptrt->r3[1];
		for(int i = 0; i < 16; i++){	ptrt->apx[i] = pxr[i];		}
	}
}

void iteration_shift(Task *ptrt, Setting *ptrs, int num)
{
	int check, range;
	
	rangetest.push_back(ptrt->mean);	rangetest.push_back(range);
	
	// inital
	ptrt->r4[0] = 0;	
	if(ptrs->shift_block == 0){ptrt->r4[1] = ptrt->sd * 2;	}
	else{	ptrt->r4[1] = ptrs->shift_block;}	
	
	cout << "	-----------------------------------------------------------------" << endl;
	check = ptrt->after;
	
	for(int i = 1; i < 10; i++)
	{
		ptrt->iteration = i;
		cout << num+1 << "/" << ptrt->f << "/" << ptrt->n << "	|	iteration " << i << ":	" << " | r4: " << ptrt->r4[0] << "," << ptrt->r4[1];
		
		compression(ptrt, ptrs, true);
		getcodeword(ptrt, ptrs, true, false);
		levels.clear();	pxs.clear();	vector <int>().swap(levels);	vector <int>().swap(pxs);
		ptrt->after = countafter();		rangetest.push_back(ptrt->after);
	
		outcomecompair(ptrt, ptrs);
		ptrt->r4[0]++; ptrt->r4[1]++;
		cout << "	| after: " << ptrt->after << "		| best: " << ptrt->best[4];	
		cout << endl << "	-----------------------------------------------------------------" << endl;
	}
	rangeCSV(ptrt, ptrs, num);
}

void iteration_random(Task *ptrt, Setting *ptrs, int num)
{
	int check, tmean = ptrt->mean, tsd = ptrt->sd;
	srand(time(NULL));	
	cout << "	-----------------------------------------------------------------" << endl;
	check = ptrt->after;
	
	for(int i = 1; i < 6; i++)		// for every compression round
	{
		// initial 
		//srand( time(NULL) );
		ptrt->r4[0] = tmean - rand() % tsd;	ptrt->r4[1] = tmean + rand() % tsd;
		ptrt->r3[0] = ptrt->r4[0] - rand() % tsd;	ptrt->r3[1] = ptrt->r4[1] + rand() % tsd + 1;
		if(ptrt->r4[0] <= 0){ptrt->r4[0] = 0; ptrt->r3[0] = 0;}
		if(ptrt->r3[0] < 0){ptrt->r3[0] = 0;}
		ptrt->mean = 0;	ptrt->sd = 0;	ptrt->encodetimes = 0;
		
		ptrt->iteration = i;
		cout << num+1 << "/" << ptrt->f << "/" << ptrt->n << "	|	iteration " << i << ":	" << " | r4: " << ptrt->r4[0] << "," << ptrt->r4[1];
		
		compression(ptrt, ptrs, true);
		//analysis(ptrt);
		getcodeword(ptrt, ptrs, true, false);
		ptrt->after = countafter();
	
		//outputCSV(ptrt, num);
	
		outcomecompair(ptrt, ptrs);
		
		//cout << "	| after: " << ptrt->after << "	| ";
		if(ptrt->after < check){	cout << '-' << check - ptrt->after;}
		else{	cout << '+' << ptrt->after - check;	}
		
		cout << endl << "	-----------------------------------------------------------------" << endl;
	}	
}

void iteration_diffusion(Task *ptrt, Setting *ptrs,int num)
{
	int check, range = ptrt->sd * 10 + 1;
	
	rangetest.push_back(ptrt->mean);	rangetest.push_back(range);
	
	// inital
	if(ptrs->mid_type == 0){ptrt->r4[0] = ptrt->mean;	ptrt->r4[1] = ptrt->mean;	}
	else if(ptrs->mid_type == 1){ptrt->r4[0] = ptrt->mid;	ptrt->r4[1] = ptrt->mid;	}
	ptrt->mean = 0;	ptrt->sd = 0;	ptrt->encodetimes = 0;	ptrt->nectimes = 0;	ptrt->mid = 0;
	
	cout << "	-----------------------------------------------------------------" << endl;
	check = ptrt->after;
	
	for(int i = 1; i < range; i++)
	{
		ptrt->iteration = i;	ptrt->r4[0]--; ptrt->r4[1]++;	if(ptrt->r4[0] < 0){ptrt->r4[0] = 0;}
		cout << num+1 << "/" << ptrt->f << "/" << ptrt->n << "	|	iteration " << i << ":	" << " | r4: " << ptrt->r4[0] << "," << ptrt->r4[1];
		
		compression(ptrt, ptrs, true);
		getcodeword(ptrt, ptrs, true, false);
		levels.clear();	pxs.clear();	vector <int>().swap(levels);	vector <int>().swap(pxs);
		ptrt->after = countafter();		rangetest.push_back(ptrt->after);
	
		outcomecompair(ptrt, ptrs);
		cout << "	| after: " << ptrt->after << "		| best: " << ptrt->best[4];	
		cout << endl << "	-----------------------------------------------------------------" << endl;
	}
	rangeCSV(ptrt, ptrs, num);
}

void iteration_from0(Task *ptrt, Setting *ptrs,int num)
{
	int range = ptrt->temp_level;
	ptrt->r4[0] = 0;	ptrt->r4[1] = 0;
	
	for(int i = 1; i < range; i++)
	{
		ptrt->iteration = i;	ptrt->r4[1] = i;
		cout << num+1 << "/" << ptrt->f << "/" << ptrt->n << "	|	iteration " << i << ":	" << " | r4: " << ptrt->r4[0] << "," << ptrt->r4[1];
		
		if(levelrecord[i] == 0){cout << "	|	(skip)		|";	
		cout << endl << "	-----------------------------------------------------------------" << endl;	continue;	}
		
		// px rearange
		if(ptrs->px_mode > 3){	reArangePx(ptrt->detail_recoed, ptrt->r4[1] + ptrt->f, ptrs->limit);	}
		
		compression(ptrt, ptrs, true);			//cout << "compression ok" << endl;
		getcodeword(ptrt, ptrs, true, false);	//cout << "getcodeword ok" << endl; 
		
		// clear record
		levels.clear();	pxs.clear();	vector <int>().swap(levels);	vector <int>().swap(pxs);
		ptrt->after = countafter();		rangetest.push_back(ptrt->after);
	
		outcomecompair(ptrt, ptrs);
		cout << "	| after: " << ptrt->after << "		| best: " << ptrt->best[4];	
		cout << endl << "	-----------------------------------------------------------------" << endl;
		levelrecord[i] = 0;
	}
	rangeCSV(ptrt, ptrs, num);
}

void choiceIterationType(Task *ptrt, Setting *ptrs,int num)
{
	if(ptrs->fix_level_bit){ptrt->temp_level = ptrt->lmax;}
	else{	/* do nothing*/	}
	
	// iteration
	if(ptrs->iter_type == 2){iteration_random(ptrt, ptrs, ptrt->num);	}
	else if(ptrs->iter_type == 1){iteration_diffusion(ptrt, ptrs, ptrt->num);}
	else if(ptrs->iter_type == 3){iteration_shift(ptrt, ptrs, ptrt->num);	}
	else if(ptrs->iter_type == 4){iteration_from0(ptrt, ptrs, ptrt->num);	}
	else{} // do nothing		
}

void round0(Task* ptrt, Setting *ptrs)
{
	//initial
	ptrt->lmax = 0;
	ptrt->temp_level = ptrs->limit;
	
	compression(ptrt, ptrs, false); 		//cout << "Compression OK!" << endl;
	getcodeword(ptrt, ptrs, false, true);	//cout << "Encode OK!" << endl;
	cout << "	-----------------------------------------------------------------" << endl;
	analysis(ptrt);
	
	cout << "	| Ave: " << ptrt->mean;
	cout << "	| SD: " << ptrt->sd;
	cout << "		| Mid: " << ptrt->mid << "		| " << endl;

	// get compression data of one cube
	ptrt->after = countafter();
	if(ptrs->px_mode > 1 && ptrs->px_mode < 4){sortpx(ptrs->px_mode);}
	outputCSV(ptrt, ptrs, ptrt->num);
			
	outcomecompair(ptrt, ptrs);
	
	choiceIterationType(ptrt, ptrs, ptrt->num);
}   

void forloop(int a, Task *ptrt, Setting *ptrs)
{
	int temp[4];
	if(a == 1)
	{
		std::cout <<"		";
		for(int r = 0; r < 16; r++){std::cout << pxr[r] << ", ";	}
		std::cout << std::endl << "	-------------------------------------------------------------------------" << std::endl;
		round0(ptrt, ptrs);
	}
	else
	{
		for(int j = 0; j < a; j++)
		{
			temp[0] = pxr[0];	temp[1] = pxr[1];	temp[2] = pxr[2];	temp[3] = pxr[3];
			for(int i = 1; i < a; i++)
			{
				pxr[i*4-4] = pxr[i*4];	pxr[i*4+1-4] = pxr[i*4+1];
				pxr[i*4+2-4] = pxr[i*4+2];	pxr[i*4+3-4] = pxr[i*4+3];
			}
			pxr[a*4-4] = temp[0];		pxr[a*4-3] = temp[1];
			pxr[a*4-2] = temp[2];		pxr[a*4-1] = temp[3]; 	
			
			forloop(a-1, ptrt, ptrs);
		}
	}
}

void handlefile(Setting *ptrs, Task* ptrt, int num)
{
	getdata(ptrt->name, ptrt);
	ptrt->best[4] = ptrt->total;
	
	// initial
	recordtable(ptrs->limit, ptrs->pb);
	initialCSV(ptrt, ptrs, num);	initial_rangeCSV(ptrt, ptrs, num);
	
	// floor loop
	for(int i = ptrs->a[0]; i < ptrs->a[1]; i++)
	{
		cout << "	-----------------------------------------------------------------" << endl;
		ptrt->f = i;
		dataArang(ptrt, ptrs);
		
		// not encode bit loop
		for(int j = ptrs->b[0]; j < ptrs->b[1]; j++)
		{
			cout << "	-----------------------------------------------------------------" << endl;
			// initial normal compression
			ptrt->detail_recoed = (int*)calloc(ptrs->limit * pow(2, ptrs->pb), sizeof(int));
			ptrt->n = j;	ptrt->encodetimes = 0;	ptrt->mean = 0;	ptrt->sd = 0;	ptrt->iteration = 0; ptrt->num = num;
			for(int p = 0; p < 16; p++){pxr[p] = p;	}
			cout << num+1 << "	| f: " << ptrt->f << " | n: " << ptrt->n << endl;
			
			//	and all iteration
			if(ptrs->px_mode == 1){forloop(4, ptrt, ptrs);	}
			else{round0(ptrt, ptrs);	} 
			
			delete [] ptrt->detail_recoed;
		}
		delete [] ptrt->datas;
		vector <int>().swap(head);
		vector <int>().swap(butt);
	}
	cout << endl << "	-----------------------------------------------------------------" << endl;
	cout << "	" << ptrt->best[1] << "/" << ptrt->best[2] << "	best: " << ptrt->best[4] << endl; 
	
	output_txt(ptrt);
	output_vec(ptrs, ptrt);
	
	outcomes.push_back(ptrt->total);	outcomes.push_back(ptrt->best[4]);
	outcomes.push_back(ptrt->best[3]);	outcomes.push_back(ptrt->best[2]);
	outcomes.push_back(ptrt->best[1]);	outcomes.push_back(ptrt->best[0]);
	for(int r= 0; r < 16 ; r++){recordpx[num][r] = ptrt->apx[r];	}
	for(int q = 0; q < 4; q++){recordr[num][q] = ptrt->recoedrange[q];	}
	
	for(int i = 0; i < cubes.size(); i++){delete [] cubes[i];	}
	vector <int*>().swap(cubes);
	vector <int>().swap(dontcare);
}

//=================================================== main ================================================================
int main(int argc, char** argv)
{
	// for record
	int mode; 
	double start, end;
	Task *ptrt;
	Setting *ptrs;

	LoadNames();
	ptrs = new Setting;		setting(ptrs);
	initial_txt(ptrs);
	
	// choose data
	cout << "Please input test mode: ";	cin >> mode;
	if(mode == 1)
	{
		for(int i = 0; i < 6; i++)
		{
			ptrt = new Task;
			ptrt->start = clock();
			ptrt->name = (char*)malloc(sizeof(char) * fnames[i].size());
			ptrt->name = (char*)fnames[i].data();
			
			handlefile(ptrs, ptrt, i);	cout << fnames[i] << " OK! " << endl << endl;
			
			delete ptrt;
			
			cout << "	Time: " << (ptrt->end - ptrt->start) / CLOCKS_PER_SEC << " sec." << endl;
		}
	}
	else if(mode == 0){	ptrt->name = "Min/sample.vec";}
	else{	cout << "Unvalid!!!" << endl;	exit(1);	}
	
	outcome();
	
	exit(0);
}
