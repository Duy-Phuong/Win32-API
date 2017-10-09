#pragma once
#define WIN32_LEAN_AND_MEAN	

#include "resource.h"
#include "CDrive1.h"
#include "CDrive.h"
#include "windows.h"

#include <stdlib.h>
#include <tchar.h>
#include <malloc.h>
#include <memory.h>
#include "CAddress.h"
#include <crtdbg.h>
//Vị trí 8 icon cơ bản trong file Shell32.dll
#define IDI_SHELL32_FOLDER 3
#define IDI_SHELL32_UNKNOWN_FILE 0 //Chú ý khác với Tree-View là FOLDER_SELECTED
#define IDI_SHELL32_DESKTOP 34
#define IDI_SHELL32_MYCOMPUTER 15
#define IDI_SHELL32_FOLDER_SELECTED 4 

#define IDI_SHELL32_FLOPPY 6
#define IDI_SHELL32_USB 7
#define IDI_SHELL32_HDD 8
#define IDI_SHELL32_CD  11 

//Vị trí trong image list sau khi add
#define IDI_FOLDER 0
#define IDI_UNKNOWN_FILE 1
#define IDI_DESKTOP 2
#define IDI_MYCOMPUTER 3

#define IDI_FLOPPY 4
#define IDI_USB 5
#define IDI_HDD 6
#define IDI_CD  7 

#define INITIAL_ICON_IN_TREE 8 //Tổng số icon lúc đầu trong tree, mặc định chỉ nạp những gì cần thiết
#define NUMBER_OF_ICON_TO_GROW 65535 //Số icon có thể mở rộng
