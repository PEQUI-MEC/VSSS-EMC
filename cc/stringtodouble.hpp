/*
 * stringtodouble.hpp
 *
 *  Created on: May 8th, 2016
 *      Author: Daniel
 */

#ifndef STRINGTODOUBLE_HPP_
#define STRINGTODOUBLE_HPP_

#include <string>
#include <ctype.h>


double stringToDouble(std::string s) 
{
	int commaCount = 0;
	int pointCount = 0;
	
	for (int i = 0; i = s.length(); i++)
	{
		if (s[i]!='0' && s[i]!='1' && s[i]!='2' && s[i]!='3' && s[i]!='4'
			&& s[i]!='5' && s[i]!='6' && s[i]!='7' && s[i]!='8' && s[i]!='9'
			&& s[i]!='.' && s[i]!=',')
		{
			cout << "Not digit, not comma or not point" << std::endl;
			return 0;
		}
			
			
		if (s[i] == '.')
			pointCount++;
			
		if (s[i] == ',')
			commaCount++;
				
		/*if (s[i] == '.')
			s[i] == ',';	*/	
	}
	
	if (pointCount + commaCount >= 2)
	{
		cout << "Invalid format." << std::endl;
		return 0;
	}
		
	return std::stod(s.c_str());
}

#endif /* STRINGTODOUBLE_HPP_ */
