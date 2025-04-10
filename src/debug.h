//====== Assertion/Debug output APIs =================================

//#include <platform.h> // for __endexcept
#include "pch.h"

#pragma warning (disable:4096)      // '__cdecl' must be used with '...'
#pragma warning (disable:4201)      // nonstandard extension used : nameless struct/union
#pragma warning (disable:4115)      // named type definition in parentheses

#if defined(DECLARE_DEBUG) && defined(DEBUG)

//
// Declare module-specific debug strings
//
//   When including this header in your private header file, do not
//   define DECLARE_DEBUG.  But do define DECLARE_DEBUG in one of the
//   source files in your project, and then include this header file.
//
//   You may also define the following:
//
//      SZ_DEBUGINI     - the .ini file used to set debug flags
//      SZ_DEBUGSECTION - the section in the .ini file specific to
//                        the module component.
//      SZ_MODULE       - ansi version of the name of your module.
//
//

// (These are deliberately CHAR)
EXTERN_C const CHAR c_szCcshellIniFile[] = SZ_DEBUGINI;
EXTERN_C const CHAR c_szCcshellIniSecDebug[] = SZ_DEBUGSECTION;

EXTERN_C const WCHAR c_wszTrace[] = L"t " TEXTW(SZ_MODULE) L"  ";
EXTERN_C const WCHAR c_wszErrorDbg[] = L"err " TEXTW(SZ_MODULE) L"  ";
EXTERN_C const WCHAR c_wszWarningDbg[] = L"wn " TEXTW(SZ_MODULE) L"  ";
EXTERN_C const WCHAR c_wszAssertMsg[] = TEXTW(SZ_MODULE) L"  Assert: ";
EXTERN_C const WCHAR c_wszAssertFailed[] = TEXTW(SZ_MODULE) L"  Assert %ls, line %d: (%ls)\r\n";
EXTERN_C const WCHAR c_wszRip[] = TEXTW(SZ_MODULE) L"  RIP in %s at %s, line %d: (%s)\r\n";
EXTERN_C const WCHAR c_wszRipNoFn[] = TEXTW(SZ_MODULE) L"  RIP at %s, line %d: (%s)\r\n";

// (These are deliberately CHAR)
EXTERN_C const CHAR  c_szTrace[] = "t " SZ_MODULE "  ";
EXTERN_C const CHAR  c_szErrorDbg[] = "err " SZ_MODULE "  ";
EXTERN_C const CHAR  c_szWarningDbg[] = "wn " SZ_MODULE "  ";
EXTERN_C const CHAR  c_szAssertMsg[] = SZ_MODULE "  Assert: ";
EXTERN_C const CHAR  c_szAssertFailed[] = SZ_MODULE "  Assert %s, line %d: (%s)\r\n";
EXTERN_C const CHAR  c_szRip[] = SZ_MODULE "  RIP in %s at %s, line %d: (%s)\r\n";
EXTERN_C const CHAR  c_szRipNoFn[] = SZ_MODULE "  RIP at %s, line %d: (%s)\r\n";
EXTERN_C const CHAR  c_szRipMsg[] = SZ_MODULE "  RIP: ";

#endif  // DECLARE_DEBUG && DEBUG

#if defined(DECLARE_DEBUG) && defined(PRODUCT_PROF)
EXTERN_C const CHAR c_szCcshellIniFile[] = SZ_DEBUGINI;
EXTERN_C const CHAR c_szCcshellIniSecDebug[] = SZ_DEBUGSECTION;
#endif



#ifdef __cplusplus
extern "C" {
#endif

#if !defined(DECLARE_DEBUG)

//
// Debug macros and validation code
//

#if !defined(UNIX) || (defined(UNIX) && !defined(NOSHELLDEBUG))

// Undefine the macros that we define in case some other header
// might have tried defining these commonly-named macros.
#undef Assert
#undef AssertE
#undef AssertMsg
#undef AssertStrLen
#undef DebugMsg
#undef FullDebugMsg
#undef ASSERT
#undef EVAL
#undef ASSERTMSG            // catch people's typos
#undef DBEXEC

#ifdef _ATL_NO_DEBUG_CRT
#undef _ASSERTE             // we substitute this ATL macro
#endif

#endif // !UNIX


// Access these globals to determine which debug flags are set.
// These globals are modified by CcshellGetDebugFlags(), which
// reads an .ini file and sets the appropriate flags.
//
//   g_dwDumpFlags  - bits are application specific.  Typically 
//                    used for dumping structures.
//   g_dwBreakFlags - uses BF_* flags.  The remaining bits are
//                    application specific.  Used to determine
//                    when to break into the debugger.
//   g_qwTraceFlags - uses TF_* flags.  The remaining bits are
//                    application specific.  Used to display
//                    debug trace messages.
//   g_dwFuncTraceFlags - bits are application specific.  When
//                    TF_FUNC is set, CcshellFuncMsg uses this
//                    value to determine which function traces
//                    to display.
//   g_dwProtoype   - bits are application specific.  Use it for
//                    anything.
//   g_dwProfileCAP - bits are application specific. Used to
//                    control ICECAP profiling. 
//

extern DWORD g_dwDumpFlags;
extern DWORD g_dwBreakFlags;
extern ULONGLONG g_qwTraceFlags;
extern DWORD g_dwFuncTraceFlags;

// Break flags for g_dwBreakFlags
#define BF_ASSERT           0x00000001      // Break on assertions
#define BF_ONAPIENTER       0x00000002      // Break on entering an API
#define BF_ONERRORMSG       0x00000004      // Break on TF_ERROR
#define BF_ONWARNMSG        0x00000008      // Break on TF_WARNING
#define BF_THR              0x00000100      // Break when THR() receives a failure
#define BF_RIP              0x00000200      // Break on RIPs
#define BF_LEAKS            0x80000000      // Break on detecting a leak

// Trace flags for g_qwTraceFlags
#define TF_ALWAYS           0xFFFFFFFFFFFFFFFF
#define TF_NEVER            0x00000000
#define TF_WARNING          0x00000001
#define TF_ERROR            0x00000002
#define TF_GENERAL          0x00000004      // Standard messages
#define TF_FUNC             0x00000008      // Trace function calls
#define TF_ATL              0x00000008      // Since TF_FUNC is so-little used, I'm overloading this bit
#define TF_MEMUSAGE                       0x0000000100000000      
#define TF_KEEP_ALLOCATION_STACKS         0x0000000200000000      
// (Upper 28 bits reserved for custom use per-module)

#define TF_CUSTOM1          0x40000000      // Custom messages #1
#define TF_CUSTOM2          0x80000000      // Custom messages #2

// Old, archaic debug flags.  
// APPCOMPAT (scotth): the following flags will be phased out over time.
#ifdef DM_TRACE
#undef DM_TRACE
#undef DM_WARNING
#undef DM_ERROR
#endif
#define DM_TRACE            TF_GENERAL      // OBSOLETE Trace messages
#define DM_WARNING          TF_WARNING      // OBSOLETE Warning
#define DM_ERROR            TF_ERROR        // OBSOLETE Error


// Use this macro to declare message text that will be placed
// in the CODE segment (useful if DS is getting full)
//
// Ex: DEBUGTEXT(szMsg, "Invalid whatever: %d");
//
#define DEBUGTEXT(sz, msg)      /* ;Internal */ \
    static const TCHAR sz[] = msg


#ifndef NOSHELLDEBUG    // Others have own versions of these.

#ifdef DEBUG

void    AttachUserModeDebugger (void);

#ifdef _X86_
// Use int 3 so we stop immediately in the source
#define DEBUG_BREAK                                     \
    {                                                   \
        static BOOL gAlwaysAssert = FALSE;              \
        AttachUserModeDebugger();                       \
        do                                              \
        {                                               \
            _try                                        \
            {                                           \
                _asm int 3                              \
            }                                           \
            _except (EXCEPTION_EXECUTE_HANDLER)         \
            {                                           \
            }                                           \
        } while (gAlwaysAssert);                        \
    }
#else
#define DEBUG_BREAK                                     \
    {                                                   \
        static BOOL gAlwaysAssert = FALSE;              \
        AttachUserModeDebugger();                       \
        do                                              \
        {                                               \
            __try                                        \
            {                                           \
                DebugBreak();                           \
            }                                           \
            __except (EXCEPTION_EXECUTE_HANDLER)         \
            {                                           \
            }                                           \
        } while (gAlwaysAssert);                        \
    }
#endif

// Prototypes for debug functions

void CcshellStackEnter(void);
void CcshellStackLeave(void);

void CDECL CcshellFuncMsgW(ULONGLONG mask, LPCSTR pszMsg, ...);
void CDECL CcshellFuncMsgA(ULONGLONG mask, LPCSTR pszMsg, ...);
void CDECL _AssertMsgA(BOOL f, LPCSTR pszMsg, ...);
void CDECL _AssertMsgW(BOOL f, LPCWSTR pszMsg, ...);


void _AssertStrLenA(LPCSTR pszStr, int iLen);
void _AssertStrLenW(LPCWSTR pwzStr, int iLen);

#ifdef UNICODE
#define CcshellFuncMsg          CcshellFuncMsgW
#define CcshellAssertMsg        CcshellAssertMsgW
#define _AssertMsg              _AssertMsgW
#define _AssertStrLen           _AssertStrLenW
#else
#define CcshellFuncMsg          CcshellFuncMsgA
#define CcshellAssertMsg        CcshellAssertMsgA
#define _AssertMsg              _AssertMsgA
#define _AssertStrLen           _AssertStrLenA
#endif

#endif // DEBUG



// ASSERT(f)
//
//   Generates a "Assert file.c, line x (eval)" message if f is NOT true.
//
//   Use ASSERT() to check for logic invariance.  These are typically considered
//   fatal problems, and falls into the 'this should never ever happen' 
//   category.
//
//   Do *not* use ASSERT() to verify successful API calls if the APIs can 
//   legitimately fail due to low resources.  For example, LocalAlloc can 
//   legally fail, so you shouldn't assert that it will never fail.
//
//   The BF_ASSERT bit in g_dwBreakFlags governs whether the function 
//   performs a DebugBreak().
//
//   Default Behavior-
//      Retail builds:      nothing
//      Debug builds:       spew and break
//      Full debug builds:  spew and break
//
#ifdef DEBUG

BOOL CcshellAssertFailedA(LPCSTR szFile, int line, LPCSTR pszEval, BOOL bBreakInside);
BOOL CcshellAssertFailedW(LPCWSTR szFile, int line, LPCWSTR pwszEval, BOOL bBreakInside);
#ifdef UNICODE
#define CcshellAssertFailed     CcshellAssertFailedW
#else
#define CcshellAssertFailed     CcshellAssertFailedA
#endif

#define ASSERT(f)                                 \
    {                                             \
        DEBUGTEXT(szFile, TEXT(__FILE__));              \
        if (!(f) && CcshellAssertFailed(szFile, __LINE__, TEXT(#f), FALSE)) \
            DEBUG_BREAK;       \
    }

// The old Win95 code used to use "Assert()".  We discourage the use
// of this macro now because it is not msdev-friendly.
#ifdef DISALLOW_Assert
#define Assert(f)        Dont_use_Assert___Use_ASSERT
#else
#define Assert(f)           ASSERT(f)
#endif

#else  // DEBUG

#define ASSERT(f)
#define Assert(f)

#endif // DEBUG



// ASSERTMSG(f, szFmt, args...)
//
//   Behaves like ASSERT, except it prints the wsprintf-formatted message
//   instead of the file and line number.
//
//   The sz parameter is always ANSI; AssertMsg correctly converts it
//   to unicode if necessary.  This is so you don't have to wrap your
//   debug strings with TEXT().
//
//   The BF_ASSERT bit in g_dwBreakFlags governs whether the function 
//   performs a DebugBreak().
//
//   Default Behavior-
//      Retail builds:      nothing
//      Debug builds:       spew and break
//      Full debug builds:  spew and break
//
#ifdef DEBUG

void CDECL CcshellAssertMsgW(BOOL bAssert, LPCSTR pszMsg, ...);
void CDECL CcshellAssertMsgA(BOOL bAssert, LPCSTR pszMsg, ...);
#ifdef UNICODE
#define CcshellAssertMsg        CcshellAssertMsgW
#else
#define CcshellAssertMsg        CcshellAssertMsgA
#endif

#define ASSERTMSG           CcshellAssertMsg

#else  // DEBUG

#define ASSERTMSG       1 ? (void)0 : (void)

#endif // DEBUG



// EVAL(f)
//
//   Behaves like ASSERT().  Evaluates the expression (f).  The expression 
//   is always evaluated, even in retail builds.  But the macro only asserts 
//   in the debug build.  This macro may be used on logical expressions, eg:
//
//          if (EVAL(exp))
//              // do something
//
//   Do *not* use EVAL() to verify successful API calls if the APIs can 
//   legitimately fail due to low resources.  For example, LocalAlloc can 
//   legally fail, so you shouldn't assert that it will never fail.
//
//   The BF_ASSERT bit in g_dwBreakFlags governs whether the function 
//   performs a DebugBreak().
//
//   Default Behavior-
//      Retail builds:      nothing
//      Debug builds:       spew and break
//      Full debug builds:  spew and break
//
#ifdef DEBUG

#define EVAL(exp)   \
    ((exp) || (CcshellAssertFailed(TEXT(__FILE__), __LINE__, TEXT(#exp), TRUE), 0))

#else  // DEBUG

#define EVAL(exp)       ((exp) != 0)

#endif // DEBUG



// THR(pfn)
// TBOOL(pfn)
// TINT(pfn)
// TPTR(pfn)
// TW32(pfn)
// 
//   These macros are useful to trace failed calls to functions that return
//   HRESULTs, BOOLs, ints, or pointers.  An example use of this is:
//
//   {
//       ...
//       hres = THR(CoCreateInstance(CLSID_Bar, NULL, CLSCTX_INPROC_SERVER, 
//                                   IID_IBar, (LPVOID*)&pbar));
//       if (SUCCEEDED(hres))
//       ...
//   }
//
//   If CoCreateInstance failed, you would see spew similar to:
//
//    err MODULE  THR: Failure of "CoCreateInstance(CLSID_Bar, NULL, CLSCTX_INPROC_SERVER, IID_IBar, (LPVOID*)&pbar)" at foo.cpp, line 100  (0x80004005)
//
//   THR keys off of the failure code of the hresult.
//   TBOOL considers FALSE to be a failure case.
//   TINT considers -1 to be a failure case.
//   TPTR considers NULL to be a failure case.
//   TW32 keys off the failure code of the Win32 error code.
//
//   Set the BF_THR bit in g_dwBreakFlags to stop when these macros see a failure.
//
//   Default Behavior-
//      Retail builds:      nothing
//      Debug builds:       nothing
//      Full debug builds:  spew on error
//
#ifdef DEBUG

EXTERN_C HRESULT TraceHR(HRESULT hrTest, LPCSTR pszExpr, LPCSTR pszFile, int iLine);
EXTERN_C BOOL    TraceBool(BOOL bTest, LPCSTR pszExpr, LPCSTR pszFile, int iLine);
EXTERN_C int     TraceInt(int iTest, LPCSTR pszExpr, LPCSTR pszFile, int iLine);
EXTERN_C LPVOID  TracePtr(LPVOID pvTest, LPCSTR pszExpr, LPCSTR pszFile, int iLine);
EXTERN_C DWORD   TraceWin32(DWORD dwTest, LPCSTR pszExpr, LPCSTR pszFile, int iLine);

#define THR(x)      (TraceHR((x), #x, __FILE__, __LINE__))
#define TBOOL(x)    (TraceBool((x), #x, __FILE__, __LINE__))
#define TINT(x)     (TraceInt((x), #x, __FILE__, __LINE__))
#define TPTR(x)     (TracePtr((x), #x, __FILE__, __LINE__))
#define TW32(x)     (TraceWin32((x), #x, __FILE__, __LINE__))

#else  // DEBUG

#define THR(x)          (x)
#define TBOOL(x)        (x)
#define TINT(x)         (x)
#define TPTR(x)         (x)
#define TW32(x)         (x)

#endif // DEBUG



// DBEXEC(flg, expr)
//
//   under DEBUG, does "if (flg) expr;" (w/ the usual safe syntax)
//   under !DEBUG, does nothing (and does not evaluate either of its args)
//
#ifdef DEBUG

#define DBEXEC(flg, expr)    ((flg) ? (expr) : 0)

#else  // DEBUG

#define DBEXEC(flg, expr)   /*NOTHING*/

#endif // DEBUG


// string and buffer whacking functions
//


#define DEBUGWhackPathBufferA(psz, cch)
#define DEBUGWhackPathBufferW(psz, cch)
#define DEBUGWhackPathStringA(psz, cch)
#define DEBUGWhackPathStringW(psz, cch)


#ifdef UNICODE
#define DEBUGWhackPathBuffer DEBUGWhackPathBufferW
#define DEBUGWhackPathString DEBUGWhackPathStringW
#else
#define DEBUGWhackPathBuffer DEBUGWhackPathBufferA
#define DEBUGWhackPathString DEBUGWhackPathStringA
#endif


// Some trickery to map ATL debug macros to ours, so ATL code that stops
// or spews in our code will look like the rest of our squirties.

#ifdef DEBUG

#ifdef _ATL_NO_DEBUG_CRT
// ATL uses _ASSERTE.  Map it to ours.
#define _ASSERTE(f)         ASSERT(f)

// We map ATLTRACE macros to our functions
void _cdecl ShellAtlTraceA(LPCSTR lpszFormat, ...);
void _cdecl ShellAtlTraceW(LPCWSTR lpszFormat, ...);
#ifdef UNICODE
#define ShellAtlTrace   ShellAtlTraceW
#else
#define ShellAtlTrace   ShellAtlTraceA
#endif
// These are turned off because they normally don't give
// feedback of error cases and so many fire that they
// swamp out other useful debug spew.
//#define ATLTRACE            ShellAtlTrace
#endif

#else  // DEBUG

#ifdef _ATL_NO_DEBUG_CRT
// ATL uses _ASSERTE.  Map it to ours.
#define _ASSERTE(f)

// We map ATLTRACE macros to our functions
#define ATLTRACE            1 ? (void)0 : (void)
#define ATLTRACE2           1 ? (void)0 : (void)
#endif

#endif // DEBUG


// ------ Stay away from these macros below ----------
// APPCOMPAT (scotth):  remove these by 8/15/98.  They should not be used anymore. 
#ifdef DEBUG

#define AssertE(f)          ASSERT(f)
#define AssertMsg           _AssertMsg
#define AssertStrLen        _AssertStrLen
#define AssertStrLenA       _AssertStrLenA
#define AssertStrLenW       _AssertStrLenW

#ifdef FULL_DEBUG
#define FullDebugMsg        _DebugMsg
#else
#define FullDebugMsg        1 ? (void)0 : (void)
#endif

#endif // DEBUG
// ------ Stay away from these macros above ----------



// It's necessary to find when classes that were designed to be single threaded are used
// across threads so they can be fixed to be multithreaded.  These asserts will point
// out such cases.
#ifdef DEBUG
#define ASSERT_SINGLE_THREADED              NULL;
#define INIT_SINGLE_THREADED_ASSERT         NULL;
#define SINGLE_THREADED_MEMBER_VARIABLE     
#endif // DEBUG



#ifdef DEBUG

#define Dbg_SafeStrA(psz)   (SAFECAST(psz, LPCSTR), (psz) ? (psz) : "NULL string")
#define Dbg_SafeStrW(psz)   (SAFECAST(psz, LPCWSTR), (psz) ? (psz) : L"NULL string")
#ifdef UNICODE
#define Dbg_SafeStr         Dbg_SafeStrW
#else
#define Dbg_SafeStr         Dbg_SafeStrA
#endif

#define FUNC_MSG            CcshellFuncMsg


// Helpful macro for mapping manifest constants to strings.  Assumes
// return string is pcsz.  You can use this macro in this fashion:
//
// LPCSTR Dbg_GetFoo(FOO foo)
// {
//    LPCTSTR pcsz = TEXT("Unknown <foo>");
//    switch (foo)
//    {
//    STRING_CASE(FOOVALUE1);
//    STRING_CASE(FOOVALUE2);
//    ...
//    }
//    return pcsz;
// }
//
#define STRING_CASE(val)               case val: pcsz = TEXT(#val); break


// Debug function enter


// DBG_ENTER(flag, fn)  -- Generates a function entry debug spew for
//                          a function
//
#define DBG_ENTER(flagFTF, fn)                  \
        (FUNC_MSG(flagFTF, " > " #fn "()"), \
         CcshellStackEnter())

// DBG_ENTER_TYPE(flag, fn, dw, pfnStrFromType)  -- Generates a function entry debug
//                          spew for functions that accept <type>.
//
#define DBG_ENTER_TYPE(flagFTF, fn, dw, pfnStrFromType)                   \
        (FUNC_MSG(flagFTF, " < " #fn "(..., %s, ...)", (LPCTSTR)pfnStrFromType(dw)), \
         CcshellStackEnter())

// DBG_ENTER_SZ(flag, fn, sz)  -- Generates a function entry debug spew for
//                          a function that accepts a string as one of its
//                          parameters.
//
#define DBG_ENTER_SZ(flagFTF, fn, sz)                  \
        (FUNC_MSG(flagFTF, " > " #fn "(..., \"%s\",...)", Dbg_SafeStr(sz)), \
         CcshellStackEnter())


// Debug function exit


// DBG_EXIT(flag, fn)  -- Generates a function exit debug spew
//
#define DBG_EXIT(flagFTF, fn)                              \
        (CcshellStackLeave(), \
         FUNC_MSG(flagFTF, " < " #fn "()"))

// DBG_EXIT_TYPE(flag, fn, dw, pfnStrFromType)  -- Generates a function exit debug
//                          spew for functions that return <type>.
//
#define DBG_EXIT_TYPE(flagFTF, fn, dw, pfnStrFromType)                   \
        (CcshellStackLeave(), \
         FUNC_MSG(flagFTF, " < " #fn "() with %s", (LPCTSTR)pfnStrFromType(dw)))

// DBG_EXIT_INT(flag, fn, us)  -- Generates a function exit debug spew for
//                          functions that return an INT.
//
#define DBG_EXIT_INT(flagFTF, fn, n)                       \
        (CcshellStackLeave(), \
         FUNC_MSG(flagFTF, " < " #fn "() with %d", (int)(n)))

// DBG_EXIT_BOOL(flag, fn, b)  -- Generates a function exit debug spew for
//                          functions that return a boolean.
//
#define DBG_EXIT_BOOL(flagFTF, fn, b)                      \
        (CcshellStackLeave(), \
         FUNC_MSG(flagFTF, " < " #fn "() with %s", (b) ? (LPTSTR)TEXT("TRUE") : (LPTSTR)TEXT("FALSE")))

// DBG_EXIT_UL(flag, fn, ul)  -- Generates a function exit debug spew for
//                          functions that return a ULONG.
//
#ifdef _WIN64
#define DBG_EXIT_UL(flagFTF, fn, ul)                   \
        (CcshellStackLeave(), \
         FUNC_MSG(flagFTF, " < " #fn "() with %#016I64x", (ULONG_PTR)(ul)))
#else
#define DBG_EXIT_UL(flagFTF, fn, ul)                   \
        (CcshellStackLeave(), \
         FUNC_MSG(flagFTF, " < " #fn "() with %#08lx", (ULONG)(ul)))
#endif // _WIN64

#define DBG_EXIT_DWORD      DBG_EXIT_UL

// DBG_EXIT_HRES(flag, fn, hres)  -- Generates a function exit debug spew for
//                          functions that return an HRESULT.
//
#define DBG_EXIT_HRES(flagFTF, fn, hres)     DBG_EXIT_TYPE(flagFTF, fn, hres, Dbg_GetHRESULTName)



#else   // DEBUG

#endif  // DEBUG



// COMPILETIME_ASSERT(f)
//
//  Generates a build break at compile time if the constant expression
//  is not true.  Unlike the "#if" compile-time directive, the expression
//  in COMPILETIME_ASSERT() is allowed to use "sizeof".
//
//  Compiler magic!  If the expression "f" is FALSE, then you get the
//  compiler error "Duplicate case expression in switch statement".
//
#define COMPILETIME_ASSERT(f) switch (0) case 0: case f:


#else  // NOSHELLDEBUG

#ifdef UNIX
#include <crtdbg.h>
#define ASSERT(f)	_ASSERT(f)
#include <mainwin.h>
#define TraceMsg(type, sformat)  DebugMessage(0, sformat)
#define TraceMSG(type, sformat, args)  DebugMessage(0, sformat, args)
#endif

#endif  // NOSHELLDEBUG


// 
// Debug dump helper functions
//


#define Dbg_GetCFName(ucf)          (void)0
#define Dbg_GetHRESULTName(hr)      (void)0
#define Dbg_GetREFIIDName(riid)     (void)0
#define Dbg_GetVTName(vt)           (void)0


// I'm a lazy typist...
#define Dbg_GetHRESULT              Dbg_GetHRESULTName

// Parameter validation macros
//#include "validate.h"

#endif // DECLARE_DEBUG

#ifdef PRODUCT_PROF 
int __stdcall StartCAP(void);	// start profiling
int __stdcall StopCAP(void);    // stop profiling until StartCAP
int __stdcall SuspendCAP(void); // suspend profiling until ResumeCAP
int __stdcall ResumeCAP(void);  // resume profiling
int __stdcall StartCAPAll(void);    // process-wide start profiling
int __stdcall StopCAPAll(void);     // process-wide stop profiling
int __stdcall SuspendCAPAll(void);  // process-wide suspend profiling
int __stdcall ResumeCAPAll(void);   // process-wide resume profiling
void __stdcall MarkCAP(long lMark);  // write mark to MEA
extern DWORD g_dwProfileCAP;
#else
#define StartCAP()      0
#define StopCAP()       0
#define SuspendCAP()    0
#define ResumeCAP()     0
#define StartCAPAll()   0
#define StopCAPAll()    0
#define SuspendCAPAll() 0
#define ResumeCAPAll()  0
#define MarkCAP(n)      0

#define g_dwProfileCAP  0
#endif

#ifdef __cplusplus
};
#endif
