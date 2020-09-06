#pragma once
#ifndef _WINDOWSTRUCTURES_H_
#define _WINDOWSTRUCTURES_H_
#include <Windows.h>
#define WM_SWAP_BUFFERS (WM_USER + 1)

enum SizeMode {
	SIZEBLE,
	NOT_SIZEBLE,
	FULL_SCREEN
};
#define VK_A 65
#define VK_B 66
#define VK_D 68
#define VK_W 87
#define VK_S 83

#endif