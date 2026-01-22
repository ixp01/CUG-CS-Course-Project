#include "pch.h"
#include "StuInfo.h"

StuInfo::StuInfo()
{
}

StuInfo::StuInfo(int Sno,string Sname, string Ssex, int Sage, int Dno)
{
	 m_Sname=Sname;
	 m_Ssex=Ssex;
     m_Sage=Sage;
	 m_Dno=Dno;
	 m_Sno=Sno;
}
