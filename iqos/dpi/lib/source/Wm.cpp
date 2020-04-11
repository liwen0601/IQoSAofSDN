/***********************************************************
 * Author: Wen Li
 * Date  : 4/05/2019
 * Describe: pattern matching: WM algorighm
 * History:
   <1> 4/05/2019 , create
************************************************************/
#include <Wm.h>

const static DWORD BLOCK_SIZE = 3;

void WmMatch::Compile(T_Pid2Pattern *Patterns) 
{
    InitPatterns (Patterns);

	cout << "===> m_Min: " << m_Min <<endl;

	for (auto ItP = Patterns->begin(), EndP = Patterns->end(); ItP != EndP; ItP++) 
    {
        string *Ptn = &(ItP->second);
		for (DWORD Ch = 0; Ch < m_Min - BLOCK_SIZE + 1; ++Ch) 
        {
			string Block = Ptn->substr(Ch, BLOCK_SIZE);
            
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

T_Result* WmMatch::Search(const string& Text, const DWORD Length)
{
    if (m_Initialized == false)
    {
        return NULL;
    }
    
    m_Result.clear ();
    
    for (DWORD Pos = m_Min - BLOCK_SIZE; Pos < Length; ++Pos) 
    {
        string block = Text.substr(Pos, BLOCK_SIZE);
        
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
            string *Pattern = &m_Patterns[Pid];

            DWORD CurPos = Pos - m_Min + BLOCK_SIZE;
            DWORD Index = 0;
            DWORD Length = Pattern->length();
            while (Index < Length) 
            {
                if (Pattern->at(Index) != Text.at(CurPos + Index)) 
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


	return &m_Result;
}
