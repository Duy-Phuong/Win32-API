#include "stdafx.h"
#include "CAddress.h"

#include "tchar.h"
#include <windowsx.h>

CAddress::CAddress()
{
	m_hInst = NULL;
	m_hParent = NULL;
	m_hAddress = NULL;
	m_nID = 0;
}

CAddress::~CAddress()
{

}

void CAddress::Create(HWND hParent, long ID, HINSTANCE hInst)
{
	InitCommonControls();
	m_hInst = hInst;
	m_hParent = hParent;
	m_hAddress = CreateWindow(TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | TBSTYLE_LIST,
		0, 0, 0, 0,
		m_hParent, (HMENU)ID, m_hInst, NULL);
	m_nID = ID;
	SetButtonImages();
	AddNonButtonControl(_T("STATIC"), _T("LINK: "), WS_CHILD | WS_VISIBLE,
		IDC_ADDRESS_STATIC, 50, 50, 0, IDC_ADDRESS_STATIC, 3);
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(GetDlgItem(m_hAddress, IDC_ADDRESS_STATIC), WM_SETFONT, (WPARAM)hFont, TRUE);

	AddNonButtonControl(_T("EDIT"), NULL, WS_CHILD | WS_VISIBLE, IDC_ADDRESS_EDIT, 900, 20, 1, IDC_ADDRESS_EDIT, 1);
	SendMessage(GetDlgItem(m_hAddress, IDC_ADDRESS_EDIT), WM_SETFONT, (WPARAM)hFont, TRUE);

}

void CAddress::SetButtonImages()
{
	SendMessage(m_hAddress, TB_SETBITMAPSIZE, 0, MAKELONG(16, 16));

	HIMAGELIST hIml = ImageList_Create(16, 16, ILC_MASK | ILC_COLOR32, 1, 0);
	SendMessage(m_hAddress, TB_SETIMAGELIST, 0, (LPARAM)hIml);
}

HWND CAddress::AddNonButtonControl(LPTSTR className, LPTSTR caption, int styles, int ID, int width, int height, int btnIndex, int iCommand, int lechTop)
{
	TBBUTTON button;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = BTNS_SEP;
	button.idCommand = iCommand;
	BOOL kq = SendMessage(m_hAddress, TB_INSERTBUTTON, btnIndex, (LPARAM)&button);

	TBBUTTONINFO info;
	info.cbSize = sizeof(TBBUTTONINFO);
	info.dwMask = TBIF_SIZE;
	info.cx = width;
	SendMessage(m_hAddress, TB_SETBUTTONINFO, iCommand, (LPARAM)&info);

	RECT rect;
	SendMessage(m_hAddress, TB_GETITEMRECT, btnIndex, (LPARAM)&rect);

	HWND hWnd;
	hWnd = CreateWindow(className, caption, styles, rect.left, rect.top + lechTop,
		rect.right - rect.left, height == 0 ? rect.bottom - rect.top : height,
		m_hAddress, (HMENU)ID, m_hInst, 0);

	return hWnd;
}

HWND CAddress::GetHandle()
{
	return m_hAddress;
}
