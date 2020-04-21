/***********************************************************
 * Author: Wen Li
 * Date  : 4/05/2019
 * Describe: pattern matching: WM algorighm
 * History:
   <1> 4/05/2019 , create
************************************************************/
#include <Wm.h>
#include <Log.h>

const static DWORD BLOCK_SIZE = 3;

void WmMatch::Compile(T_Pid2Pattern *Patterns) 
{
	for (auto ItP = Patterns->begin(), EndP = Patterns->end(); ItP != EndP; ItP++) 
    {
        BYTE *Ptn = (BYTE*)((ItP->second).c_str());
		for (DWORD Ch = 0; Ch < m_Min - BLOCK_SIZE + 1; ++Ch) 
        {
			DWORD Block = Ptn[Ch]<<16 | Ptn[Ch+1]<<8 | Ptn[Ch+2];
            
			DWORD BlockPos = m_Min - Ch - BLOCK_SIZE;
			DWORD Shift = (BlockPos == 0) ? (m_Min - BLOCK_SIZE + 1) : BlockPos;

			/* Shift table */
			if (m_ShiftTable.count(Block)) 
            {
				Shift = m_ShiftTable[Block];
				m_ShiftTable[Block] = min(m_ShiftTable[Block], BlockPos);
			}
			else
			{
				m_ShiftTable[Block] = BlockPos;
			}

			/* HASH table */
			if (BlockPos != 0) 
            {
                continue;
			}

            m_HashTable[Block].push_back(ItP->first);
		    if (Shift != 0) 
            {
			    if (m_AuxShiftTable.count(Block))
			    {
				    m_AuxShiftTable[Block] = min(m_AuxShiftTable[Block], Shift);
			    }
			    else
			    {
				    m_AuxShiftTable[Block] = Shift;
			    }
			}
		}
	}

	m_Initialized = true;
}

T_Result* WmMatch::Search(const BYTE* Text, const DWORD Length)
{
    if (m_Initialized == false)
    {
        return NULL;
    }
    
    m_Result.clear ();
    
    for (DWORD Pos = m_Min - BLOCK_SIZE; Pos < Length; ++Pos) 
    {
        DWORD block = Text[Pos]<<16 | Text[Pos+1]<<8 |Text[Pos+2];
        
        auto shift_value = m_ShiftTable.find(block);
        if (shift_value == m_ShiftTable.end()) 
        {
            Pos += m_Min - BLOCK_SIZE;
            continue;
        }

        if (shift_value->second != 0)
        {
            Pos += shift_value->second - 1;
            continue;
        }


        /* hit */
        auto PtSet = m_HashTable.at(block);
        for (auto PIt = PtSet.begin(), PEnd = PtSet.end(); PIt != PEnd; ++PIt) 
        {
            DWORD Pid = *PIt;
            string *Pattern = &(m_Patterns[Pid]);

            DWORD CurPos = Pos - m_Min + BLOCK_SIZE;
            DWORD Index = 0;

            BYTE* Pt = (BYTE*)Pattern->c_str();
            DWORD Length = Pattern->length();

            const BYTE* Tg = Text + CurPos;
            while (Index < Length) 
            {
                if (*Pt++ != *Tg++) 
                {
                    break;
                }
                
                ++Index;
            }

            if (Index == Length) 
            {
                m_Result.insert (Pid);
            }
        }
                
        Pos += m_AuxShiftTable.at(block) - 1;
    }


    for (auto it = m_Result.begin(); it != m_Result.end(); it++)
    {
        DWORD Id = *it;
        DebugLog ("Pattern Matching => [%d]%s\r\n", Id, m_Patterns[Id].c_str());
    }


	return &m_Result;
}


VOID WmTest ()
{
    T_Pid2Pattern Patterns;
    Patterns[1] = "www.liwen.cn";
    Patterns[2] = "helloworld";
    Patterns[3] = "facebook";
    Patterns[4] = "zoom space";

    WmMatch WmTest (&Patterns);

    const char*  Data = "123456666www.liwen.cn----helloworldgotoschoolfacebook";
    T_Result *Res = WmTest.Search ((BYTE*)Data, strlen(Data));
    assert (Res != NULL);

    assert (Res->find (1) != Res->end());
    assert (Res->find (2) != Res->end());
    assert (Res->find (3) != Res->end());

    cout<<"Wm Test ok!!!\r\n";
        
}

