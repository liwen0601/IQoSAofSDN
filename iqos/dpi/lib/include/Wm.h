/***********************************************************
 * Author: Wen Li
 * Date  : 4/05/2019
 * Describe: pattern matching: WM algorighm
 * History:
   <1> 4/05/2019 , create
************************************************************/

#ifndef _WM_H_
#define _WM_H_
#include <BasicType.h> 

using namespace std;

typedef unordered_map<DWORD, DWORD> T_String2Lengh;
typedef unordered_map<DWORD, list<DWORD>> T_Hash;

typedef unordered_map<DWORD, string> T_Pid2Pattern;
typedef set<DWORD> T_Result;


class WmMatch 
{
public:
	WmMatch(T_Pid2Pattern* Patterns)
	{
        m_Min = 0xffffffff;
        m_Initialized = false;
        
        InitPatterns (Patterns);

        Compile (Patterns);
	}
    
	~WmMatch()
	{
	}
    
	
	T_Result* Search(const BYTE* Text, const DWORD Length);

    DWORD WmTest ();

private:
    VOID Compile(T_Pid2Pattern* Patterns);
    
private:
	bool m_Initialized;
	DWORD m_Min;

	T_String2Lengh m_ShiftTable;
	T_String2Lengh m_AuxShiftTable;
	T_Hash m_HashTable;

    
    T_Result m_Result;
    T_Pid2Pattern m_Patterns;

    inline VOID InitPatterns (T_Pid2Pattern* Patterns)
    {
        for (auto it = Patterns->begin(), end = Patterns->end(); it != end; it++)
        {
            string Ptn = it->second;
            m_Patterns[it->first] = it->second;

            DWORD Len = Ptn.length ();
            if (m_Min > Len)
            {
                m_Min = Len;
            }
        }

        return;
    }

};


#endif
