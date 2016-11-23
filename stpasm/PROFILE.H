#if !defined(AFX_PROFILER_H__118301E0_1AA6_11D6_A32E_0004E202CA99__INCLUDED_)
#define AFX_PROFILER_H__118301E0_1AA6_11D6_A32E_0004E202CA99__INCLUDED_


//log file item struct
struct LOGITEM {
	unsigned short Count;		//Number of bytes in per record
	unsigned short Line;		//Line number in source file
	unsigned int  ProcessID;	//Current process ID
	unsigned int  ThreadID;	//Current thread ID
	__int64 Cycles;		//NULL for function start
	LPSTR  Source;		//Source file path and name
	LPSTR  Function;	//Function that was profiled
}; 

//log file item struct
//This structure is used as statistics return
//And used for pcp comparison
struct LOGSTATUS {
	__int64 MaxCycles;	//Maximum number of cycles expended
	BYTE   NumThread;	//Number of threads
	BYTE   NumProcess;	//Number of processes
	LPSTR  PopFunc[5];	//5 Most popular functions 
	UINT   Count[5];	//Number of times they were called
	UINT   MaxCount;	//Number of items or funtion calls in log
}; 

//Profiler class

class CProfile  
{
public:
	//Constructor
	CProfile(LPSTR token, LPSTR strFile, USHORT nLine)
	{ 
		//Intialize members
		m_Token = token;	//Function name
		m_Line  = nLine;    //Current line
		m_File  = strFile;	//Current source

		//Use local variable to reference class member
		unsigned __int64 nStart;	

		__asm cpuid						
		__asm rdtsc						
		__asm mov dword ptr [nStart+0], eax
		__asm mov dword ptr [nStart+4], edx
		m_nTotal = nStart;	//Cheat a little burn off 4 intructions
	}
	
	//Destructor
	~CProfile()
	{ 
		unsigned __int64 nEnd;	//End cycle count

		__asm cpuid					
		__asm rdtsc					
		__asm mov dword ptr [nEnd+0], eax	//Read low
		__asm mov dword ptr [nEnd+4], edx	//Read hi	 


		//Calculate current length
		USHORT nRecLen = ((strlen(m_Token)+strlen(m_File)+sizeof(LOGITEM)) - (sizeof(LPSTR)*2)) - 2;

		//Initalize logitem structure
		
		LOGITEM logItem = {nRecLen,		
						   m_Line,			  
						   GetCurrentProcessId(), 
						   GetCurrentThreadId(),  
						   nEnd-m_nTotal,
						   m_File,			  
						   m_Token};		
		
		//Write to log file 
		CFile myFile("profile.pcp", CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate);	//Open file for writting only
		myFile.SeekToEnd();		//Start at end of file
		myFile.Write(&logItem.Count,     sizeof(USHORT));
		myFile.Write(&logItem.Line,      sizeof(USHORT));
		myFile.Write(&logItem.ProcessID, sizeof(DWORD));
		myFile.Write(&logItem.ThreadID,  sizeof(DWORD));
		myFile.Write(&logItem.Cycles,    sizeof(__int64));
		myFile.Write(logItem.Source,    strlen(logItem.Source)+1);
		myFile.Write(logItem.Function,  strlen(logItem.Function)+1);
		myFile.Close();
		
	}
private:
	//Start and finsih clock ticks
	unsigned __int64 m_nTotal;
	
	//Location information
	LPSTR		 m_Token;
	LPSTR		 m_File;
	USHORT		 m_Line;
};

//CProfile useage requires only single declartion at beginning of function

#if _DEBUG
	
	#define PROFILE(token) CProfile _tagPROFILE_CLASS(#token, THIS_FILE, __LINE__)
#else
	#define PROFILE(token) ((void)0)		//Release
	//#define AfxInitPCSpectraProfiler(file)((void)0)
#endif



#endif // !defined(AFX_PROFILER_H__118301E0_1AA6_11D6_A32E_0004E202CA99__INCLUDED_)
