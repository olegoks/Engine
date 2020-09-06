#pragma once

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <Windows.h>
#include "WindowStructures.h"
#include <stdlib.h>
#include <string>
#include <chrono>
#include <iostream>
#include <tchar.h>
#include <windows.h>
#include "Structures.h"
#include "__memory_managers_/MemoryManagerCPU.h"
#include "wchar_tFunctions.h"
#include <vector>


	namespace wnd {

		struct Size {

			unsigned int height;
			unsigned int width;

		};

		struct win_coordinats {
		
			unsigned int x;
			unsigned int y;

		};
		using namespace std::chrono;
		class Window {
		private:

			static Window* This;
			Size size_;
			RgbPixel* display_buffer_;
			//RgbPixel* now_is_displaying_buffer_;
			DISPLAYBUFFERINFO display_buffer_info_;
			const unsigned int kBitsPerPixel = 32;
			wchar_t* win_caption;
			win_coordinats win_coordinats_;
			HWND Handle;
			HINSTANCE AppIntanceHandle;
			HDC DeviceContext;
			SizeMode sizeMode;
			high_resolution_clock::time_point first_time_point_count_fps_;
			std::vector<Keystroke> keys_;
			static void PushKeystroke(KeyType key);

		public:

			static	LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM  wParam, LPARAM lParam);
			Window(HINSTANCE appIntanceHandle);
			~Window();
			void SetDisplayBuffer(RgbPixel* display_buffer, const unsigned int display_buffer_size);

			void DisplayFrame();
			std::vector<Keystroke>* GetKeystrokesBuffer() noexcept { return &keys_; }
			RgbPixel* GetHostDisplayBuffer() const noexcept { return display_buffer_; };
			void SetMode(int mode);
			void SetCoordinats(unsigned int x, unsigned int y);
			void SetSize(unsigned int height, unsigned int width);
			int Create();
			inline void SetWindowCaption(wchar_t* caption) noexcept { this->win_caption = caption; };
			bool Show();
			int StartMessageLoop();
			bool RegisterWindowClass();

			Size GetSize();
			HDC GetDeviceContext();
			HWND GetHandle();
			win_coordinats GetCoord();
			void SetSizeMode(SizeMode sizeMode) { this->sizeMode = sizeMode; };
		};

	};

#endif // !_WINDOW_H_