#include "pch.h"
#include "cowsite.h"

HRESULT CObjectWithSite::SetSite(IUnknown* punkSite)
{
	IUnknown_Set(&_punkSite, punkSite);
	return S_OK;
}

HRESULT CObjectWithSite::GetSite(REFIID riid, void** ppvSite)
{
	if (_punkSite)
		return _punkSite->QueryInterface(riid, ppvSite);

	*ppvSite = NULL;
	return E_FAIL;
}

HRESULT CSafeServiceSite::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] = {
		QITABENT(CSafeServiceSite, IServiceProvider),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}

ULONG CSafeServiceSite::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

ULONG CSafeServiceSite::Release()
{
	ASSERT(0 != _cRef);
	ULONG cRef = InterlockedDecrement(&_cRef);
	if (0 == cRef)
	{
		delete this;
	}
	return cRef;
}

HRESULT CSafeServiceSite::QueryService(REFGUID guidService, REFIID riid, void** ppvObj)
{
	if (_psp)
		return _psp->QueryService(guidService, riid, ppvObj);

	*ppvObj = NULL;
	return E_NOINTERFACE;
}

HRESULT CSafeServiceSite::SetProviderWeakRef(IServiceProvider* psp)
{
	_psp = psp;
	return S_OK;
}