// no struct use

#include "math.h"

char FindChar(int a)
{
	if(a == 0){return '0';	}	else if(a == 1){return '1';	}
	else if(a == 2){return '2';	}	else if(a == 3){return '3';	}
	else if(a == 4){return '4';	}	else if(a == 5){return '5';	}
	else if(a == 6){return '6';	}	else if(a == 7){return '7';	}
	else if(a == 8){return '8';	}	else if(a == 9){return '9';	}
	else{	std::cout << "Invalid in FindCHar(int a) !!!" << std::endl;	exit(1);}
}

int GetDec(int *ptrh, int n)
{
	int temp = 0, ex = 0;
	
	for(int i = n; i > -1; i--)
	{
		if(*(ptrh + i) == 2){	ex++;	continue;	}
		else{	temp = temp + *(ptrh + i) * pow(2, ex);	ex++;	}
	}
	return temp;
}

std::string GetBin(int target, int len)
{
	std::string temp = "";
	int r[len] = {0};
	int c = len;

	while(target > 0)
	{
		r[c - 1] = target % 2;
		target = target / 2;
		c--;
	}
	
	for(int i = 0; i < len; i++)
	{
		if(r[i] == 0){temp = temp + '0';}
		else{temp = temp + '1';	}
	}
	
	return temp;
}
