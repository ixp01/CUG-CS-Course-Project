#pragma once
#include<string>
class StuInfo
{
public:
	string m_Sname;
	string m_Ssex;
	int m_Sage;
	int m_Dno;
	int m_Sno;

	StuInfo();
	StuInfo(int Sno,string Sname, string Ssex, int Sage, int Dno);
};

