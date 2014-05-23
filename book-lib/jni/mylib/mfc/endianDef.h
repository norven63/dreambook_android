#pragma once
enum emEndian
{
	emUnknownEndian = 0,
	emBigEndian,
	emLittelEndian
};
class cEndianDef
{

public:
	static cEndianDef* Instance()
	{
		static cEndianDef endian;
		return (&endian);
	}
	emEndian GetEndian()
	{
		return m_emEndian;
	}
private:
	union
	{ 
		short  s; 
		char   c[sizeof(short)]; 
	} un; 
	cEndianDef()
	{
		m_emEndian=emUnknownEndian;
		un.s = 0x0102; 
		if (sizeof(short) == 2)
		{ 
			if (un.c[0] == 1 && un.c[1] == 2) 
				m_emEndian = emBigEndian;
			else if (un.c[0] == 2 && un.c[1] == 1) 
				m_emEndian = emLittelEndian;
			else 
				;
		}
	}
private:
	emEndian m_emEndian;
};