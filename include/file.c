//==============================================================================
//
// Title:		file.c
// Purpose:		A short description of the implementation.
//
// Created on:	2023-03-08 at 10:17:29 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

//#include "file.h"
#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/// HIFN  What does your function do?
/// HIPAR x/What inputs does your function expect?
/// HIRET What does your function return?
//int Define_Your_Functions_Here (int x)
//{
//	return x;
//}

void SplitPath(const char* pathName, char *driveName, char *dirName, char *fileName)
{
	char c[4][300] = { 0 };
	_splitpath(pathName, c[0], c[1], c[2], c[3]);
	if (NULL != driveName)
	{
		sprintf(driveName, "%s", c[0]);
	}
	if (NULL != dirName)
	{
		sprintf(dirName, "%s", c[1]);
	}
	if (NULL != fileName)
	{
		sprintf(fileName, "%s%s", c[2], c[3]);
	}
	//printf("%s\n%s\n%s\n%s\n", c[0], c[1], c[2], c[3]);
}

void GetCurrentAllFile(const char *searchDir, char fileList[][300], int *numOfFile)
{
	int i = 0;
	DIR *d;
	struct dirent *dir;
	d = opendir(searchDir);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (DT_REG == dir->d_type)
			{
				sprintf(fileList[i], "%s\\%s", searchDir, dir->d_name);
				i++;
			}
		}
		closedir(d);
	}
	*numOfFile = i;
}

void GetCurrentAllDir(const char *searchDir, char dirList[][300], int *numOfDir)
{
	int i = 0;
	DIR *d;
	struct dirent *dir;
	d = opendir(searchDir);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (DT_DIR == dir->d_type)
			{
				//printf("%s ", dir->d_name);
				sprintf(dirList[i], "%s\\%s", searchDir, dir->d_name);
				i++;
			}
		}
		closedir(d);
	}
	*numOfDir = i;
}