
// no struct use

#include "math.h"

int pxr[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

std::vector<int> levels;		// recode levels of this round
std::vector<int> pxs;		// record patterns of this round

void sortpx(int mode)
{
	int max = 0, temp;
	int newpxr[16];
	
	// sorted
	for(int i = 0; i < 16; i++)
	{
		// find max
		for(int j = 0; j < 16; j++){if(pxrecord[j] > max){temp = j; max = pxrecord[j];}	}
		newpxr[i] = temp;
		pxrecord[temp] = 0;
		max = 0;
	}
	for(int k = 0; k < 16; k++){pxr[k] = newpxr[k];	}
}

//=================================================================================================
bool p1234test(int failodd, int faileven, int *ptrh, int numofbits)
{
	bool odd = true;
	for(int i = 0; i < numofbits; i++)
	{
		if(odd){if(*(ptrh + i) == failodd){return false;}	odd = false;}
		else{if(*(ptrh + i) == faileven){return false;}	odd = true;	}
	}
	return true;
}

bool layerfail(int layerbits, int *ptrl, int failodd, int faileven)
{
	bool odd = true;
	
	for(int i = 0; i < layerbits; i++)
	{
		if(odd){if(*(ptrl + i) == failodd){return true;	}	odd = false;}
		else{if(*(ptrl + i) == faileven){return true;	}	odd = true;	}
	}
	return false;
}

bool g1p5678test(int *ptrh, int numofbits, int level, int px)
{
	int middle = (level / 2) - 1;			// find the level which num of bit = the next layer
	int type = level % 2;					// 0 = even level, 1 = odd level
	int headbit = 0;						// show first bit position of this layer
	int layerbits = 1;						// show how many bits in this layer
	bool oddlayer = true;					// show layer type
	
	for(int i = 0; i < level; i++)			// handle layer by layer
	{
		if(oddlayer)						// odd layer
		{
			if(px == 5 && layerfail(layerbits, ptrh + headbit, 1, 1)){return false;}
			if(px == 6 && layerfail(layerbits, ptrh + headbit, 0, 0)){return false;}
			if(px == 7 && layerfail(layerbits, ptrh + headbit, 1, 0)){return false;}
			if(px == 8 && layerfail(layerbits, ptrh + headbit, 0, 1)){return false;}
			oddlayer = false;
		}
		else								// even layer
		{
			if(px == 5 && layerfail(layerbits, ptrh + headbit, 0, 0)){return false;}
			if(px == 6 && layerfail(layerbits, ptrh + headbit, 1, 1)){return false;}
			if(px == 7 && layerfail(layerbits, ptrh + headbit, 0, 1)){return false;}
			if(px == 8 && layerfail(layerbits, ptrh + headbit, 1, 0)){return false;}
			oddlayer = true;
		}
		
		// fix headbit
		headbit = headbit + layerbits;
		
		// fix layer bits
		if(type == 0 && i == middle){continue;	}
		
		if(i > middle){layerbits--;	}
		else{layerbits++;	}
	}
	return true;
}

bool g1uptest(int *ptrh, int layers, int px)
{
	bool oddlayer = true;
	int headbit = 0;						// show first bit position of this layer
	int layerbits = 1;						// show how many bits in this layer
	
	for(int i = 0; i < layers; i++)			// handle layer by layer
	{
		if(oddlayer)						// odd layer
		{
			if(px == 13 && layerfail(layerbits, ptrh + headbit, 1, 1)){return false;}
			if(px == 14 && layerfail(layerbits, ptrh + headbit, 0, 0)){return false;}
			if(px == 15 && layerfail(layerbits, ptrh + headbit, 1, 0)){return false;}
			if(px == 16 && layerfail(layerbits, ptrh + headbit, 0, 1)){return false;}
			oddlayer = false;
		}
		else								// even layer
		{
			if(px == 13 && layerfail(layerbits, ptrh + headbit, 0, 0)){return false;}
			if(px == 14 && layerfail(layerbits, ptrh + headbit, 1, 1)){return false;}
			if(px == 15 && layerfail(layerbits, ptrh + headbit, 0, 1)){return false;}
			if(px == 16 && layerfail(layerbits, ptrh + headbit, 1, 0)){return false;}
			oddlayer = true;
		}
		// fix headbit
		headbit = headbit + layerbits;
		
		// fix layer bits
		layerbits++;
	}
	return true;
}

bool g1downtest(int *ptrh, int layers, int px)
{
	bool oddlayer = true;
	int headbit = 0;						// show first bit position of this layer
	int layerbits = layers;						// show how many bits in this layer
	
	for(int i = 0; i < layers; i++)			// handle layer by layer
	{
		if(oddlayer)						// odd layer
		{
			if(px == 13 && layerfail(layerbits, ptrh + headbit, 1, 1)){return false;}
			if(px == 14 && layerfail(layerbits, ptrh + headbit, 0, 0)){return false;}
			if(px == 15 && layerfail(layerbits, ptrh + headbit, 1, 0)){return false;}
			if(px == 16 && layerfail(layerbits, ptrh + headbit, 0, 1)){return false;}
			oddlayer = false;
		}
		else								// even layer
		{
			if(px == 13 && layerfail(layerbits, ptrh + headbit, 0, 0)){return false;}
			if(px == 14 && layerfail(layerbits, ptrh + headbit, 1, 1)){return false;}
			if(px == 15 && layerfail(layerbits, ptrh + headbit, 0, 1)){return false;}
			if(px == 16 && layerfail(layerbits, ptrh + headbit, 1, 0)){return false;}
			oddlayer = true;
		}
		// fix headbit
		headbit = headbit + layerbits;
		
		// fix layer bits
		layerbits--;
	}
	return true;
}

bool g1p90123456test(int *ptrh, int numofbits, int level, int px)
{
	int up, down;	// middle layer always cover by down
	int uplayers = (level + 1) / 2;
	int downlayers = level / 2;
	
	if(level % 2 == 1)		// odd
	{
		up = (numofbits - (level + 1) / 2) / 2;
		down = (numofbits - (level + 1) / 2) / 2 + (level + 1) / 2;
	}
	else					// even
	{
		up = numofbits / 2;
		down = numofbits / 2;
	}
	
	switch(px)
	{
	case 9:			// up 0, down 1
		if(!p1234test(1, 1, ptrh, up)){return false;}
		if(!p1234test(0, 0, ptrh + up, down)){return false;}
		break;
	case 10:		// up 1, down 0
		if(!p1234test(0, 0, ptrh, up)){return false;}
		if(!p1234test(1, 1, ptrh + up, down)){return false;}
		break;
	case 11:		// up 01, down 10
		if(!p1234test(1, 0, ptrh, up)){return false;}
		if(!p1234test(0, 1, ptrh + up, down)){return false;}
		break;
	case 12:		// up 10, down 01
		if(!p1234test(0, 1, ptrh, up)){return false;}
		if(!p1234test(1, 0, ptrh + up, down)){return false;}
		break;
	
	case 13:		// up 01 layer, down 10 layer
		if(!g1uptest(ptrh, uplayers, 13)){return false;}
		if(!g1downtest(ptrh + up, downlayers, 13)){return false;}
		break;	
	case 14:		// up 10 layer, down 01 Layer 
		if(!g1uptest(ptrh, uplayers, 14)){return false;}
		if(!g1downtest(ptrh + up, downlayers, 14)){return false;}
		break;	
	case 15:		// up 01/10 Layer, down 10/01 Layer
		if(!g1uptest(ptrh, uplayers, 15)){return false;}
		if(!g1downtest(ptrh + up, downlayers, 15)){return false;}
		break;	
	case 16:		// up 10/01 Layer, down 01/10 Layer
		if(!g1uptest(ptrh, uplayers, 16)){return false;}
		if(!g1downtest(ptrh + up, downlayers, 16)){return false;}
		break;	
	}
	return true;
}

bool g2p90123456test(int *ptrh, int level, int px)
{
	int up, down;	// middle layer always cover by down
	int uplayers = level - 1;
	int downlayers = level;
	
	up = ((level - 1) *  level)/ 2;
	down = ((level + 1) *  level)/ 2;
	
	switch(px)
	{
	case 9:			// up 0, down 1
		if(!p1234test(1, 1, ptrh, up)){return false;}
		if(!p1234test(0, 0, ptrh + up, down)){return false;}
		break;
	case 10:		// up 1, down 0
		if(!p1234test(0, 0, ptrh, up)){return false;}
		if(!p1234test(1, 1, ptrh + up, down)){return false;}
		break;
	case 11:		// up 01, down 10
		if(!p1234test(1, 0, ptrh, up)){return false;}
		if(!p1234test(0, 1, ptrh + up, down)){return false;}
		break;
	case 12:		// up 10, down 01
		if(!p1234test(0, 1, ptrh, up)){return false;}
		if(!p1234test(1, 0, ptrh + up, down)){return false;}
		break;
	
	case 13:		// up 01 layer, down 10 layer
		if(!g1uptest(ptrh, uplayers, 13)){return false;}
		if(!g1downtest(ptrh + up, downlayers, 13)){return false;}
		break;	
	case 14:		// up 10 layer, down 01 Layer 
		if(!g1uptest(ptrh, uplayers, 14)){return false;}
		if(!g1downtest(ptrh + up, downlayers, 14)){return false;}
		break;	
	case 15:		// up 01/10 Layer, down 10/01 Layer
		if(!g1uptest(ptrh, uplayers, 15)){return false;}
		if(!g1downtest(ptrh + up, downlayers, 15)){return false;}
		break;	
	case 16:		// up 10/01 Layer, down 01/10 Layer
		if(!g1uptest(ptrh, uplayers, 16)){return false;}
		if(!g1downtest(ptrh + up, downlayers, 16)){return false;}
		break;	
	}
	return true;
}

bool g2p5678test(int *ptrh, int numofbits, int layer, int px)
{
	int middle = layer / 2;			// find the level which num of bit = the next layer
	int headbit = 0;						// show first bit position of this layer
	int layerbits = 1;						// show how many bits in this layer
	bool oddlayer = true;					// show layer type
	
	for(int i = 0; i < layer; i++)			// handle layer by layer
	{
		if(oddlayer)						// odd layer
		{
			if(px == 5 && layerfail(layerbits, ptrh + headbit, 1, 1)){return false;}
			if(px == 6 && layerfail(layerbits, ptrh + headbit, 0, 0)){return false;}
			if(px == 7 && layerfail(layerbits, ptrh + headbit, 1, 0)){return false;}
			if(px == 8 && layerfail(layerbits, ptrh + headbit, 0, 1)){return false;}
				
			oddlayer = false;
		}
		else								// even layer
		{
			if(px == 5 && layerfail(layerbits, ptrh + headbit, 0, 0)){return false;}
			if(px == 6 && layerfail(layerbits, ptrh + headbit, 1, 1)){return false;}
			if(px == 7 && layerfail(layerbits, ptrh + headbit, 0, 1)){return false;}
			if(px == 8 && layerfail(layerbits, ptrh + headbit, 1, 0)){return false;}
			
			oddlayer = true;
		}
		
		// fix headbit
		headbit = headbit + layerbits;
		
		if(i > middle){layerbits--;	}
		else{layerbits++;	}
	}
	return true;
}

bool g3p5678test(int *ptrh, int level, int px)
{	
	int headbit = 0;
	bool oddlayer = true;
	
	for(int i = 1; i < level + 1; i++)
	{
		if(oddlayer)						// odd layer
		{
			if(px == 5 && layerfail(i, ptrh + headbit, 1, 1)){return false;}
			if(px == 6 && layerfail(i, ptrh + headbit, 0, 0)){return false;}
			if(px == 7 && layerfail(i, ptrh + headbit, 1, 0)){return false;}
			if(px == 8 && layerfail(i, ptrh + headbit, 0, 1)){return false;}
			oddlayer = false;
		}
		else								// even layer
		{
			if(px == 5 && layerfail(i, ptrh + headbit, 0, 0)){return false;}
			if(px == 6 && layerfail(i, ptrh + headbit, 1, 1)){return false;}
			if(px == 7 && layerfail(i, ptrh + headbit, 0, 1)){return false;}
			if(px == 8 && layerfail(i, ptrh + headbit, 1, 0)){return false;}
			oddlayer = true;
		}
		
		headbit = headbit + i;
	}
	return true;
}

int graphnum(int level, int graphtype)
{
	int numofbits;						// now need to compair # of bit
	// how many bits need to compair
	if(graphtype == 1)
	{ 
		numofbits = (1 + (level / 2)) * (level / 2);
		if( (level % 2) == 1 ){numofbits = numofbits + level - ( level / 2 );}		// odd level
	}
	else if(graphtype == 2)
	{
		if(level == 1){numofbits = 3;}
		else{numofbits = pow(level, 2);	}
	}
	else if(graphtype == 3){	numofbits = ((1 + level) * level) / 2;}
	else{	numofbits = level * ceil(level / 3.0);	}
	
	return numofbits;
}

bool graph(int level, int graph, int *ptrh, int numofbits, int pb)
{	
	for(int a = 0; a < 16; a++)
	{
		switch(pxr[a])
		{
		case 0:		if(p1234test(1, 1, ptrh, numofbits))	{pxs.push_back(a);	return true;}	break;	// p1_all 0
		case 1:		if(p1234test(0, 0, ptrh, numofbits))	{pxs.push_back(a);	return true;}	break;	// p2_all 1
		case 2:		if(p1234test(1, 0, ptrh, numofbits))	{pxs.push_back(a);	return true;}	break;	// p3_01
		case 3:		if(p1234test(0, 1, ptrh, numofbits))	{pxs.push_back(a);	return true;}	break;	// p4_10
		
		case 4:		if(graph == 1 && g1p5678test(ptrh, numofbits, level, 5))		{pxs.push_back(a); 	return true;}	// p5_01 layer
					if(graph == 2 && g2p5678test(ptrh, numofbits, 2*level-1, 5))	{pxs.push_back(a);	return true;}	// p5_01 layer
					break;
					
		case 5:		if(graph == 1 && g1p5678test(ptrh, numofbits, level, 6))		{pxs.push_back(a);	return true;}	// p6_10 layer
					if(graph == 2 && g2p5678test(ptrh, numofbits, 2*level-1, 6))	{pxs.push_back(a);	return true;}	// p6_10 layer
					if(graph == 3 && g3p5678test(ptrh, level, 6))					{pxs.push_back(a);	return true;}	// p6_10 layer
					break;
					
		case 6:		if(graph == 1 && g1p5678test(ptrh, numofbits, level, 7))		{pxs.push_back(a);	return true;}	// p7_10, 01 layer
					if(graph == 2 && g2p5678test(ptrh, numofbits, 2*level-1, 7))	{pxs.push_back(a);	return true;}	// p7_10, 01 layer
					if(graph == 3 && g3p5678test(ptrh, level, 7))					{pxs.push_back(a);	return true;}	// p7_10, 01 layer
					break;
					
		case 7:		if(graph == 1 && g1p5678test(ptrh, numofbits, level, 8))		{pxs.push_back(a);	return true;}	// p8_01, 10 layer
					if(graph == 2 && g2p5678test(ptrh, numofbits, 2*level-1, 8))	{pxs.push_back(a);	return true;}	// p8_01, 10 layer
					if(graph == 3 && g3p5678test(ptrh, level, 8))					{pxs.push_back(a);	return true;}	// p8_01, 10 layer
					break;
					
		case 8:		if(graph == 1 && g1p90123456test(ptrh, numofbits, level, 9))	{pxs.push_back(a); return true;}
					if(graph == 2 && g2p90123456test(ptrh, level, 9))				{pxs.push_back(a); return true;}
					break;
		case 9:		if(graph == 1 && g1p90123456test(ptrh, numofbits, level, 10))	{pxs.push_back(a); return true;}
					if(graph == 2 && g2p90123456test(ptrh, level, 10))				{pxs.push_back(a); return true;}
					break;
		case 10:	if(graph == 1 && g1p90123456test(ptrh, numofbits, level, 11))	{pxs.push_back(a); return true;}
					if(graph == 2 && g2p90123456test(ptrh, level, 11))				{pxs.push_back(a); return true;}
					break;
		case 11:	if(graph == 1 && g1p90123456test(ptrh, numofbits, level, 12))	{pxs.push_back(a); return true;}
					if(graph == 2 && g2p90123456test(ptrh, level, 12))				{pxs.push_back(a); return true;}
					break;
					
		case 12:	if(graph == 1 && g1p90123456test(ptrh, numofbits, level, 13))	{pxs.push_back(a); return true;}
					if(graph == 2 && g2p90123456test(ptrh, level, 13))				{pxs.push_back(a); return true;}
					break;
		case 13:	if(graph == 1 && g1p90123456test(ptrh, numofbits, level, 14))	{pxs.push_back(a); return true;}
					if(graph == 2 && g2p90123456test(ptrh, level, 14))				{pxs.push_back(a); return true;}
					break;
		case 14:	if(graph == 1 && g1p90123456test(ptrh, numofbits, level, 15))	{pxs.push_back(a); return true;}
					if(graph == 2 && g2p90123456test(ptrh, level, 15))				{pxs.push_back(a); return true;}
					break;
		case 15:	if(graph == 1 && g1p90123456test(ptrh, numofbits, level, 16))	{pxs.push_back(a); return true;}
					if(graph == 2 && g2p90123456test(ptrh, level, 16))				{pxs.push_back(a); return true;}
					break;
		default:	std::cout << std::endl << "ERROR!! "  << std::endl;	exit(1);
		}
		if(pb == 2 && a == 3){break;}
		if(pb == 3 && a == 7){break;}
	}
	return false;
}
