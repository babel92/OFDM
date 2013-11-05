#ifndef JSSYNCHORNIZE_H_INCLUDED
#define JSSYNCHORNIZE_H_INCLUDED

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0700
#endif // _WIN32_WINNT

#include <windows.h>

class JSEvent
{
private:
    HANDLE m_hevent;
    bool m_duplicate;
public:
    JSEvent(bool manual=0,bool initialstatus=0,LPSECURITY_ATTRIBUTES psa=NULL,LPCSTR name=NULL);
    ~JSEvent(){if(m_duplicate==0)CloseHandle(m_hevent);}
    void Set(){SetEvent(m_hevent);}
    void Reset(){ResetEvent(m_hevent);}
    void Pulse(){PulseEvent(m_hevent);}
    void WaitForThis(DWORD time=INFINITE){WaitForSingleObject(m_hevent,time);}
};

inline JSEvent::JSEvent(bool manual,bool initialstatus,LPSECURITY_ATTRIBUTES psa,LPCSTR name)
{
    m_duplicate=0;
    m_hevent=CreateEvent(psa,manual,initialstatus,name);
    if(GetLastError()==ERROR_ALREADY_EXISTS)
        m_duplicate=1;
}

class JSCriticalSection
{
private:
    CRITICAL_SECTION m_cs;
public:
    JSCriticalSection(DWORD spincount=0){InitializeCriticalSectionAndSpinCount(&m_cs,spincount);}
    ~JSCriticalSection(){DeleteCriticalSection(&m_cs);}

    void Enter(){EnterCriticalSection(&m_cs);}
    void Leave(){LeaveCriticalSection(&m_cs);}
};

#endif // JSSYNCHORNIZE_H_INCLUDED
