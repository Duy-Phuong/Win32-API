#include "stdafx.h"
#include "CDrive1.h"


#include <shellapi.h>
//Dùng để sử dụng hàm StrCpy, StrNCat
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


Drive1::Drive1()
{
	mDriveLetter = NULL;
	mVolumeLabel = NULL;
	mDriveType = NULL;
	mNumberOfDrive = 0;
}

Drive1::~Drive1()
{
	for (int i = 0; i < mNumberOfDrive; ++i)
	{
		delete[] mDriveLetter[i];
		delete[] mVolumeLabel[i];
		delete[] mDriveType[i];
	}

	delete[] mDriveLetter;
	delete[] mVolumeLabel;
	delete[] mDriveType;
	mNumberOfDrive = 0;
}

/*****************************************************************************************/
TCHAR* Drive1::getDriveType(int position)
{
	return mDriveType[position];
}

TCHAR* Drive1::getDriveLetter(const int &i)
{
	return mDriveLetter[i];
}


TCHAR* Drive1::getDisplayName(const int &i)
{
	return mVolumeLabel[i];
}

int Drive1::getCount()
{
	return mNumberOfDrive;
}

/*****************************************************************************************/

void Drive1::getSystemDrives()
{
	TCHAR buffer[MAX_BUFFER_LEN];
	TCHAR* volumeTempName = new TCHAR[VOLUME_NAME_SIZE];
	//dem so luong o dia
	GetLogicalDriveStrings(MAX_BUFFER_LEN, buffer);

	mNumberOfDrive = 0;
	for (int i = 0; (buffer[i] != '\0') || (buffer[i - 1] != '\0'); i++) 
	{
		if (buffer[i] == '\0')
		{
			mNumberOfDrive++;
		}
	}

	//Cấp phát bộ nhớ để chứa chuỗi tương ứng
	mDriveLetter = new TCHAR*[mNumberOfDrive];
	mVolumeLabel = new TCHAR*[mNumberOfDrive];
	mDriveType = new TCHAR*[mNumberOfDrive];
	mDriveSize = new CDrive*[mNumberOfDrive];

	for (int i = 0; i < mNumberOfDrive; ++i)
	{
		mDriveLetter[i] = new TCHAR[MAX_DRIVE_LETTER];
		mVolumeLabel[i] = new TCHAR[MAX_DISPLAY_NAME];
		mDriveType[i] = new TCHAR[MAX_TYPE_LEN];
	}

	int j, k;
	int index = 0;
	//lay tung ki tu o dia
	for (j = 0; j < mNumberOfDrive; ++j)
	{
		k = 0;
		while (buffer[index] != 0)
		{
			mDriveLetter[j][k++] = buffer[index++];
		}
		mDriveLetter[j][k] = '\0'; //Kết thúc chuỗi
		++index;
	}

	//Lấy volume cho từng ổ đĩa, kết hợp luôn với việc lấy tên hiển thị ứng với từng ổ
	int nType;
	for (int i = 0; i < mNumberOfDrive; ++i)
	{
		nType = GetDriveType(mDriveLetter[i]);

		switch (nType)
		{
		case DRIVE_FIXED:
			StrCpy(mDriveType[i], FIXED_DRIVE);
			break;
		case DRIVE_REMOVABLE:
			StrCpy(mDriveType[i], REMOVABLE_DRIVE);
			break;
		case DRIVE_REMOTE:
			StrCpy(mDriveType[i], REMOTE_DRIVE);
			break;
		case DRIVE_CDROM:
			StrCpy(mDriveType[i], CD_ROM);
			break;
		default:
			break;
		}

		mDriveSize[i] = getDriveSize(i);

		StrCpy(buffer, _T(""));

		if ((nType == DRIVE_FIXED) || ((nType == DRIVE_REMOVABLE)) || (nType == DRIVE_REMOTE))
		{
			GetVolumeInformation(mDriveLetter[i], buffer, MAX_BUFFER_LEN, VOLUME_SERIAL_NUMBER, MAX_COMPONENT_LEN, FILE_SYSTEM_FLAG, FILE_SYSTEM_NAME_BUFF, FILE_SYSTEM_NAME_SIZE);
			StrCpy(volumeTempName, buffer);
		}
		else
			if (nType == DRIVE_CDROM)
			{
				GetVolumeInformation(mDriveLetter[i], buffer, MAX_BUFFER_LEN, VOLUME_SERIAL_NUMBER, MAX_COMPONENT_LEN, FILE_SYSTEM_FLAG, FILE_SYSTEM_NAME_BUFF, FILE_SYSTEM_NAME_SIZE);

				if (wcslen(buffer) == 0)
				{
					StrCpy(volumeTempName, _T("CD-ROM"));
				}
				else
					StrCpy(volumeTempName, buffer);
			}
			else if (((i == 0) || (i == 1)) && (nType == DRIVE_REMOVABLE)) 
			{
				StrCpy(volumeTempName, _T("We don't talk to floppy disk any more"));
			}

		
		if (wcslen(volumeTempName) == 0)
		{
			StrCpy(mVolumeLabel[i], _T("Local Disk"));
		}
		else
		{
			//Copy 
			StrCpy(mVolumeLabel[i], volumeTempName);
		}
		//Thêm vào phần sau hai dấu mở ngoặc. Ví dụ: (C:) như ta thường thấy WinXp (C:)

		StrCat(mVolumeLabel[i], _T(" ("));
		StrNCat(mVolumeLabel[i], mDriveLetter[i], 3);
		StrCat(mVolumeLabel[i], _T(")"));
	}
}

/*****************************************************************************************/

CDrive* Drive1::getDriveSize(int i)
{
	__int64 totalSize;
	__int64 freeSpace;

	SHGetDiskFreeSpaceEx(getDriveLetter(i), NULL, (PULARGE_INTEGER)&totalSize, (PULARGE_INTEGER)&freeSpace);

	CDrive* driveSize = new CDrive(totalSize, freeSpace);

	return driveSize;
}

LPWSTR Drive1::getFreeSpace(int i)
{
	return mDriveSize[i]->getFreeSpace();
}

LPWSTR Drive1::getTotalSize(int i)
{
	return mDriveSize[i]->getTotalSize();
}