
// no struct use 

std::string prefix(int level)
{
	std::string temp = "";
	int group, sublevel, r = 0;
	std::vector<int> remainder;
	
	// set group bit and sublevel bit
	level = level + 1;
	group = floor( log(level) / log(2) );
	sublevel = level - pow(2, group);
	
	// generate group prefix
	for(int i = 0; i < group - 1; i++){	temp = temp + "1";}
	temp = "1" + temp + "0";
	
	// generate sublevel bits
	for(int j = 0; j < group; j++)
	{
		remainder.push_back(sublevel % 2);
		r = r + 1;
		sublevel = sublevel / 2;
	}

	for(int j = remainder.size() - 1; j > -1; j--)
	{
		if(remainder[j] == 0){ temp = temp + "0";}
		else{ temp = temp + "1";	} 
	}
	return temp;
}

std::string tail(int px, int pb, int level)
{	
	if(pb == 2)
	{
		switch(px)
		{
		case 0:		return "00";	case 1:		return "01";	case 2:		return "10";	case 3:		return "11";
		default: std::cout << std::endl << "ERROR!! at " << level << " of " << px << std::endl;	exit(1);
		}
	}
	else if(pb == 3)
	{
		switch(px)
		{
		case 0:		return "000";	case 1:		return "001";	case 2:		return "010";	case 3:		return "011";	
		case 4:		return "100";	case 5:		return "101";	case 6:		return "110";	case 7:		return "111";
		default: std::cout << std::endl << "ERROR!! at " << level << " of " << px << std::endl;	exit(1);
		}	
	}
	else if(pb == 4)
	{
		switch(px)
		{
		case 0:		return "0000";	case 1:		return "0001";	case 2:		return "0010";	case 3:		return "0011";
		case 4:		return "0100";	case 5:		return "0101";	case 6:		return "0110";	case 7:		return "0111";
		case 8:		return "1000";	case 9:		return "1001";	case 10: 	return "1010";	case 11:	return "1011";
		case 12:	return "1100";	case 13:	return "1101";	case 14:	return "1110";	case 15:	return "1111";
		default: std::cout << std::endl << "ERROR!! at " << level << " of " << px << std::endl;	exit(1);
		}	
	}	
}

