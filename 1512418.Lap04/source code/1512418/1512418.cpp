// 1512418.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "1512418.h"

//#include "CDrive1.h"

#define MAX_LOADSTRING 100
#include <commctrl.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")
//For StrCpy, StrNCat
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <shellapi.h>

#define NUMBER_OF_INIT_ICON 8 
#define MAX_EXPAND_ICON 3

#define DEFAULT_ICON_INDEX 0

//For init lv column
#define LVCOL_DRIVE_TYPE		0
#define LVCOL_FOLDER_TYPE		1

//
#define MAX_PATH_LEN 10240

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
RECT rcClient;                       // The parent window's client area.
HINSTANCE g_hInstance;
HWND	g_hWnd;
HWND	g_hTreeView;
HWND	g_hListView;
Drive1* g_Drive;
CAddress* g_Address;
int myComputerIconIndex, desktopIconIndex, driveIconIndex;
int lFolderIconIndex, sFolderIconIndex, lUnknown, sUnknown;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

HWND createListView(long lExtStyle, HWND parentWnd, long ID, HINSTANCE hParentInst, int x, int y, int nWidth, int nHeight, long lStyle);
HWND createTreeView(long lExtStyle, HWND parentWnd, long ID, HINSTANCE hParentInst, int x, int y, int nWidth, int nHeight, long lStyle);
void loadMyComputerToTree(Drive1 *drive, HWND m_hTreeView);
void loadMyComputerToListView(Drive1 *drive, HWND m_hListView);
void loadExpandedChild(HTREEITEM hCurrSelected, HWND m_hTreeView); //Load tat ca thu muc con tren treeview
LPCWSTR getPath(HTREEITEM hItem, HWND m_hTreeView); 
void loadTreeviewItemAt(HTREEITEM &hParent, LPCWSTR path, HWND m_hTreeView); 
void loadListviewItemAt( LPCWSTR path, HWND m_hParent, HWND m_hListView, Drive1 *drive);
void loadOrExecSelected(HWND hWnd, HWND m_hListView);
void loadFileAndFolder(HWND m_hParent, HWND m_hListView, LPCWSTR path); 
void initListviewColumn( HWND m_hListView, int type); 
LPWSTR convertTimeStampToString(const FILETIME &ftLastWrite); 
////////************
	

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_1512418, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_1512418));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
		if (!IsDialogMessage(g_hWnd, &msg))
        //if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_1512418);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   //hInst = hInstance; // Store instance handle in our global variable
	g_hInstance = hInstance;
   HWND g_hWnd = CreateWindowW(szWindowClass, _T("FILE EXPLORER"), WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!g_hWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);

   return TRUE;
}

//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
    switch (message)
    {
	case WM_CREATE:
	{
		g_Address = new CAddress;
		g_Address->Create(hWnd, IDC_ADDRESS, g_hInstance);
		g_Drive = new Drive1();
		g_Drive->getSystemDrives();

		InitCommonControls();

		//lay kich thuoc cua so chinh
		GetClientRect(hWnd, &rcClient);
		int parentWidth = rcClient.right - rcClient.left;
		int parentHeight = rcClient.bottom - rcClient.top;

		//tao treeview
		long extStyle = 0, style = TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;
		int x = 0, y = 30;
		int nWidth = parentWidth / 4, nHeight = parentHeight;

		g_hTreeView = createTreeView(extStyle, hWnd, IDT_TREEVIEW, g_hInstance, x, y, nWidth, nHeight, style);
		///******


		////*****
		loadMyComputerToTree(g_Drive, g_hTreeView);
		SetFocus(g_hTreeView);

		//tao listview
		x = nWidth + 1;
		nWidth = (rcClient.right - rcClient.left) * 3 / 4;
		extStyle = WS_EX_CLIENTEDGE;
		style = LVS_REPORT | LVS_ICON | LVS_EDITLABELS | LVS_SHOWSELALWAYS;

		g_hListView = createListView(extStyle, hWnd, IDL_LISTVIEW, g_hInstance, x, y, nWidth, nHeight, style);
		SetDlgItemText(GetDlgItem(hWnd, IDC_ADDRESS), IDC_ADDRESS_EDIT, _T("My Computer"));

		loadMyComputerToListView(g_Drive, g_hListView);

	}
	break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_NOTIFY:
	{
		int nCurSelIndex;


		NMHDR* notifyMess = (NMHDR*)lParam; 
		LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)notifyMess; 
		HTREEITEM currSelected;

		switch (notifyMess->code)
		{
		case TVN_ITEMEXPANDING: 
			currSelected = lpnmTree->itemNew.hItem;
			loadExpandedChild(currSelected, g_hTreeView);
			break;
		case TVN_SELCHANGED:
			
			currSelected = TreeView_GetSelection(g_hTreeView); 
			TreeView_Expand(g_hTreeView, currSelected, TVE_EXPAND);

			ListView_DeleteAllItems(g_hListView); 
			loadListviewItemAt(getPath(currSelected, g_hTreeView), hWnd, g_hListView, g_Drive);
			break;

		case NM_CLICK:
			break;

		case NM_DBLCLK:
			if (notifyMess->hwndFrom == g_hListView)
				loadOrExecSelected(hWnd, g_hListView);
			break;
		}

	}
	break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


////////////////**************************----------------------------------


HWND createListView(long lExtStyle, HWND parentWnd, long ID, HINSTANCE hParentInst, int x, int y, int nWidth, int nHeight, long lStyle)
{
	//Create
	HWND m_hListView = CreateWindowEx(lExtStyle, WC_LISTVIEW, _T("List View"),
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | lStyle,
		x, y, nWidth, nHeight, parentWnd, (HMENU)ID, hParentInst, NULL);

	HIMAGELIST shiml;  
	HIMAGELIST lhiml;   
	HBITMAP hbmp;     
					  
	lhiml = ImageList_Create(30, 30, ILC_COLOR32 | ILC_MASK,
		NUMBER_OF_INIT_ICON, NUMBER_OF_ICON_TO_GROW);
	shiml = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK,
		NUMBER_OF_INIT_ICON, NUMBER_OF_ICON_TO_GROW);

	 
	hbmp = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
	lUnknown = ImageList_Add(lhiml, hbmp, (HBITMAP)NULL);  
	DeleteObject(hbmp);

	hbmp = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
	sFolderIconIndex = ImageList_Add(shiml, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);


	ListView_SetImageList(m_hListView, lhiml, LVSIL_NORMAL);
	ListView_SetImageList(m_hListView, shiml, LVSIL_SMALL);

	//Init 5 columns
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 150;
	lvCol.pszText = _T("Tên");
	ListView_InsertColumn(m_hListView, 0, &lvCol);

	lvCol.fmt = LVCFMT_LEFT;
	lvCol.pszText = _T("Loại");
	lvCol.cx = 125;
	ListView_InsertColumn(m_hListView, 1, &lvCol);

	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 125;
	lvCol.pszText = _T("Tổng dung lượng");
	ListView_InsertColumn(m_hListView, 2, &lvCol);


	lvCol.fmt = LVCFMT_LEFT;
	lvCol.pszText = _T("Dung lượng trống");
	lvCol.cx = 125;
	ListView_InsertColumn(m_hListView, 3, &lvCol);

	return m_hListView;
}

HWND createTreeView(long lExtStyle, HWND parentWnd, long ID, HINSTANCE hParentInst, int x, int y, int nWidth, int nHeight, long lStyle)
{
	//Create
	HWND m_hTreeView = CreateWindowEx(lExtStyle, WC_TREEVIEW, _T("Tree View"),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_SIZEBOX | WS_VSCROLL | WS_TABSTOP | lStyle,
		x, y, nWidth, nHeight, parentWnd,
		(HMENU)ID, hParentInst, NULL);
	
	HIMAGELIST himl;  
	HBITMAP hbmp1;     

					  // Create the image list. 
	himl = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK,
		NUMBER_OF_INIT_ICON, MAX_EXPAND_ICON);

	hbmp1 = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP4));
	desktopIconIndex = ImageList_Add(himl, hbmp1, (HBITMAP)NULL);
	
	
	TreeView_SetImageList(m_hTreeView, himl, TVSIL_NORMAL);

	///////////////////////////////////////////////////////

	return m_hTreeView;
}



void loadMyComputerToTree(Drive1 *drive, HWND m_hTreeView)
{
	TV_INSERTSTRUCT tvInsert;

	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	//Load Desktop
	tvInsert.hParent = NULL;
	tvInsert.hInsertAfter = TVI_ROOT;
	tvInsert.item.iImage = desktopIconIndex;
	tvInsert.item.iSelectedImage = desktopIconIndex;
	tvInsert.item.pszText = _T("Desktop");
	tvInsert.item.lParam = (LPARAM)_T("Desktop");
	HTREEITEM hDesktop = TreeView_InsertItem(m_hTreeView, &tvInsert);

	//Load My Computer
	tvInsert.hParent = hDesktop;
	tvInsert.hInsertAfter = TVI_LAST; 
	tvInsert.item.iImage = myComputerIconIndex;
	tvInsert.item.iSelectedImage = myComputerIconIndex;
	tvInsert.item.pszText = _T("My Computer");
	tvInsert.item.lParam = (LPARAM)_T("MyComputer");
	HTREEITEM hMyComputer = TreeView_InsertItem(m_hTreeView, &tvInsert);

	//Load volume
	for (int i = 0; i < g_Drive->getCount(); ++i)
	{
		tvInsert.hParent = hMyComputer; 
		tvInsert.item.iImage = driveIconIndex;
		tvInsert.item.iSelectedImage = driveIconIndex;
		tvInsert.item.pszText = g_Drive->getDisplayName(i); 
		tvInsert.item.lParam = (LPARAM)g_Drive->getDriveLetter(i);
		HTREEITEM hDrive = TreeView_InsertItem(m_hTreeView, &tvInsert);

		loadTreeviewItemAt(hDrive, getPath(hDrive, m_hTreeView), m_hTreeView);
	}

	TreeView_Expand(m_hTreeView, hMyComputer, TVE_EXPAND);
	TreeView_SelectItem(m_hTreeView, hMyComputer);
}


void loadMyComputerToListView(Drive1 *drive, HWND m_hListView)
{

	initListviewColumn(m_hListView, LVCOL_DRIVE_TYPE);
	LV_ITEM lv;

	for (int i = 0; i < drive->getCount(); ++i)
	{
		lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;

		lv.iItem = i;
		lv.iImage = sFolderIconIndex;
		lv.iSubItem = 0;
		lv.pszText = drive->getDisplayName(i);
		lv.lParam = (LPARAM)drive->getDriveLetter(i);
		ListView_InsertItem(m_hListView, &lv);

		//
		lv.mask = LVIF_TEXT;

		//cot 1
		lv.iSubItem = 1;
		lv.pszText = drive->getDriveType(i);
		ListView_SetItem(m_hListView, &lv);

		
		lv.iSubItem = 2;
		if (wcscmp(drive->getDriveType(i), CD_ROM) != 0)
			lv.pszText = drive->getTotalSize(i);
		else
			lv.pszText = NULL;
		ListView_SetItem(m_hListView, &lv);

		lv.iSubItem = 3;
		if (wcscmp(drive->getDriveType(i), CD_ROM) != 0)
			lv.pszText = drive->getFreeSpace(i);
		else
			lv.pszText = NULL;

		//Set
		ListView_SetItem(m_hListView, &lv);
	}
}


LPCWSTR getPath(HTREEITEM hItem, HWND m_hTreeView)
{
	TVITEMEX tv; 
	tv.mask = TVIF_PARAM;
	tv.hItem = hItem;
	TreeView_GetItem(m_hTreeView, &tv); 
	return (LPCWSTR)tv.lParam;
}

LPCWSTR getPath(HWND m_hListView, int iItem)
{
	LVITEM lv;
	lv.mask = LVIF_PARAM;
	lv.iItem = iItem;
	lv.iSubItem = 0;
	ListView_GetItem(m_hListView, &lv);
	return (LPCWSTR)lv.lParam;
}

void loadExpandedChild(HTREEITEM hCurrSelected, HWND m_hTreeView)
{
	HTREEITEM myDesktop = TreeView_GetRoot(m_hTreeView); 
	HTREEITEM myComputer = TreeView_GetChild(m_hTreeView, myDesktop); 
	if (hCurrSelected == myComputer) 
		return;

	HTREEITEM hCurrSelectedChild = TreeView_GetChild(m_hTreeView, hCurrSelected);
	if (hCurrSelectedChild != NULL)
	{
		do
		{
			if (TreeView_GetChild(m_hTreeView, hCurrSelectedChild) == NULL)
			{
				loadTreeviewItemAt(hCurrSelectedChild, getPath(hCurrSelectedChild, m_hTreeView), m_hTreeView);
			}
		} while (hCurrSelectedChild = TreeView_GetNextSibling(m_hTreeView, hCurrSelectedChild));

	}
	else
	{
		loadTreeviewItemAt(hCurrSelected, getPath(hCurrSelected, m_hTreeView), m_hTreeView);
	}
}

void loadTreeviewItemAt(HTREEITEM &hParent, LPCWSTR path, HWND m_hTreeView)
{
	//Get path
	TCHAR buffer[MAX_PATH_LEN];
	StrCpy(buffer, path); 
	StrCat(buffer, _T("\\*")); 

							 
	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = TVI_SORT;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvInsert.item.iImage = DEFAULT_ICON_INDEX;
	tvInsert.item.iSelectedImage = DEFAULT_ICON_INDEX;

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFileW(buffer, &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	TCHAR* folderPath;

	do
	{
		DWORD fileAttribute = ffd.dwFileAttributes;
		if ((fileAttribute & FILE_ATTRIBUTE_DIRECTORY) 
			&& (fileAttribute != FILE_ATTRIBUTE_HIDDEN) 
			&& (_tcscmp(ffd.cFileName, _T(".")) != 0) && (_tcscmp(ffd.cFileName, _T("..")) != 0)) 
		{
			tvInsert.item.pszText = ffd.cFileName;
			folderPath = new TCHAR[wcslen(path) + wcslen(ffd.cFileName) + 2];

			//Set path
			StrCpy(folderPath, path);
			if (wcslen(path) != 3)
				StrCat(folderPath, _T("\\"));
			StrCat(folderPath, ffd.cFileName);

			tvInsert.item.lParam = (LPARAM)folderPath;

			HTREEITEM hItem = TreeView_InsertItem(m_hTreeView, &tvInsert);
		}
	} while (FindNextFileW(hFind, &ffd));

}

void loadListviewItemAt( LPCWSTR path, HWND m_hParent, HWND m_hListView, Drive1 *drive)
{
	//If path is NULL, quit
	if (path == NULL)
		return;

	LV_ITEM lv;

	if (_tcscmp(path, _T("Desktop")) == 0)
	{
		initListviewColumn(m_hListView, LVCOL_FOLDER_TYPE);

		lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lv.iItem = 0;
		lv.iSubItem = 0;
		lv.pszText = _T("My Computer");
		lv.iImage = IDI_MYCOMPUTER;
		lv.lParam = (LPARAM)_T("MyComputer");
		ListView_InsertItem(m_hListView, &lv); 
	}
	else if (_tcscmp(path, _T("MyComputer")) == 0)
	{
		initListviewColumn(m_hListView, LVCOL_DRIVE_TYPE);

		for (int i = 0; i < drive->getCount(); ++i)
		{
			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = i;
			lv.iImage = DEFAULT_ICON_INDEX;

			lv.iSubItem = 0;
			lv.pszText = drive->getDisplayName(i);
			lv.lParam = (LPARAM)drive->getDriveLetter(i);
			ListView_InsertItem(m_hListView, &lv);

			lv.mask = LVIF_TEXT;

			
			lv.iSubItem = 1;
			lv.pszText = drive->getDriveType(i);
			ListView_SetItem(m_hListView, &lv); 

			lv.iSubItem = 2;
			if (wcscmp(drive->getDriveType(i), CD_ROM) != 0)
				lv.pszText = drive->getTotalSize(i);
			else
				lv.pszText = NULL;

			ListView_SetItem(m_hListView, &lv);

			lv.iSubItem = 3;
			if (wcscmp(drive->getDriveType(i), CD_ROM) != 0)
				lv.pszText = drive->getFreeSpace(i);
			else
				lv.pszText = NULL;

			ListView_SetItem(m_hListView, &lv);
		}
	}
	else
	{


		loadFileAndFolder(g_hWnd, m_hListView, path);
		SetDlgItemText(GetDlgItem(m_hParent, IDC_ADDRESS), IDC_ADDRESS_EDIT, path);


	}
}


void loadOrExecSelected(HWND m_hParent, HWND m_hListView)
{
	LPCWSTR filePath = getPath(m_hListView, ListView_GetSelectionMark(m_hListView));

	WIN32_FIND_DATA fd;

	if (GetFileAttributesEx(filePath, GetFileExInfoStandard, &fd) != 0)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			ListView_DeleteAllItems(m_hListView);
			loadFileAndFolder(g_hWnd, m_hListView, filePath);		
			SetDlgItemText(GetDlgItem(m_hParent, IDC_ADDRESS), IDC_ADDRESS_EDIT, filePath);

		}
		else
		{
			ShellExecute(NULL, _T("open"), filePath, NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

void loadFileAndFolder(HWND m_hParent, HWND m_hListView, LPCWSTR path)
{
		SetDlgItemText(GetDlgItem(m_hParent, IDC_ADDRESS), IDC_ADDRESS_EDIT, path);
		//////////////******************
	initListviewColumn(m_hListView, LVCOL_FOLDER_TYPE);

	TCHAR buffer[10240];

	
	StrCpy(buffer, path);

	if (wcslen(path) == 3)
		StrCat(buffer, _T("*"));
	else
		StrCat(buffer, _T("\\*"));

	//Variables
	WIN32_FIND_DATA fd; 
	HANDLE hFind = INVALID_HANDLE_VALUE;
	LV_ITEM lv;
	TCHAR* folderPath;
	int nItemCount = 0;


	hFind = FindFirstFileW(buffer, &fd);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN) &&
			(_tcscmp(fd.cFileName, _T(".")) != 0) && (_tcscmp(fd.cFileName, _T("..")) != 0)) //Ignore . (curr dir) and .. (parent dir)
		{
			folderPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(folderPath, path);

			if (wcslen(path) != 3)
				StrCat(folderPath, _T("\\"));

			StrCat(folderPath, fd.cFileName);

			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = nItemCount;
			lv.iSubItem = 0;
			lv.pszText = fd.cFileName;
			lv.iImage = DEFAULT_ICON_INDEX;
			lv.lParam = (LPARAM)folderPath;
			ListView_InsertItem(m_hListView, &lv);

			ListView_SetItemText(m_hListView, nItemCount, 1, convertTimeStampToString(fd.ftLastWriteTime));

																										  
			ListView_SetItemText(m_hListView, nItemCount, 2, _T("File folder"));

			nItemCount++;
		}

	} while (FindNextFileW(hFind, &fd));


	hFind = FindFirstFileW(buffer, &fd);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN))
		{
			folderPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(folderPath, path);

			if (wcslen(path) != 3)
				StrCat(folderPath, _T("\\"));

			StrCat(folderPath, fd.cFileName);

			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = nItemCount;
			lv.iSubItem = 0;
			lv.pszText = fd.cFileName;
			lv.lParam = (LPARAM)folderPath;

			ListView_InsertItem(m_hListView, &lv);

			ListView_SetItemText(m_hListView, nItemCount, 1, convertTimeStampToString(fd.ftLastWriteTime));

			
			DWORD fileSizeLow = fd.nFileSizeLow; 
			ListView_SetItemText(m_hListView, nItemCount, 3, CDrive::Convert(fileSizeLow));

			nItemCount++;
		}
	} while (FindNextFileW(hFind, &fd));

}

void initListviewColumn(HWND m_hListView, int type)
{
	LVCOLUMN lvCol;
	if (type == LVCOL_DRIVE_TYPE)
	{
		lvCol.mask = LVCF_TEXT | LVCF_FMT;

		lvCol.fmt = LVCFMT_LEFT | LVCF_WIDTH;
		lvCol.cx = 100;
		lvCol.pszText = _T("Loại");
		ListView_SetColumn(m_hListView, 1, &lvCol);

		lvCol.fmt = LVCFMT_RIGHT | LVCF_WIDTH;
		lvCol.cx = 80;
		lvCol.pszText = _T("Tổng dung lượng");
		ListView_SetColumn(m_hListView, 2, &lvCol);

		lvCol.cx = 80;
		lvCol.pszText = _T("Dung lượng trống");
		ListView_SetColumn(m_hListView, 3, &lvCol);
	}
	else if (type == LVCOL_FOLDER_TYPE)
	{
		lvCol.mask = LVCF_WIDTH;
		lvCol.cx = 180;
		ListView_SetColumn(m_hListView, 0, &lvCol);

		lvCol.mask = LVCF_TEXT | LVCF_FMT;
		lvCol.fmt = LVCFMT_RIGHT;
		lvCol.pszText = _T("Ngày chỉnh sửa");
		ListView_SetColumn(m_hListView, 1, &lvCol);


		lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		lvCol.fmt = LVCFMT_LEFT;
		lvCol.cx = 130;
		lvCol.pszText = _T("Loại");
		ListView_SetColumn(m_hListView, 2, &lvCol);

		lvCol.pszText = _T("Kích thước");
		ListView_SetColumn(m_hListView, 3, &lvCol);
	}
	else
	{
	}
}

LPWSTR convertTimeStampToString(const FILETIME &ftLastWrite)
{
	TCHAR *buffer = new TCHAR[50];

	SYSTEMTIME st;

	char szLocalDate[255], szLocalTime[255];

	FileTimeToSystemTime(&ftLastWrite, &st);
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_AUTOLAYOUT, &st, NULL,
		(LPWSTR)szLocalDate, 255);
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, (LPWSTR)szLocalTime, 255);

	wsprintf(buffer, L"%s %s", szLocalDate, szLocalTime);

	return buffer;
}