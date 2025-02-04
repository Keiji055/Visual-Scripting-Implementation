#pragma once
#include <Windows.h>
#include <array>
#include <stdio.h>
#include <windowsx.h>
#include <iostream>
#include "EnumKeys.h"

namespace CommonUtilities
{
	class InputHandler
	{
	public:
		InputHandler() = default;

		static bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);

		static bool IsKeyDown(const int aKeyCode);

		static bool IsKeyPressed(const int aKeyCode);

		static bool IsKeyReleased(const int aKeyCode);

		static bool IsKeyDown(const Keys aKeyCode);

		static bool IsKeyPressed(const Keys aKeyCode);

		static bool IsKeyReleased(const Keys aKeyCode);

		static POINT GetMousePosition();

		static POINT GetMouseDelta();

		static POINT GetScrollWheelDelta();

		static bool SetMousePosition(const POINT aPosition);

		static bool CaptureMouse();

		static bool UncaptureMouse();

		static void SetWindowHandle(HWND aWindowHandle);

		static void UpdateInput();

	private:
		static std::array<bool, 256> myCurrentState;		//Updated by UpdateInput
		static std::array<bool, 256> myPreviousState;		//Updated by UpdateInput
		static std::array<bool, 256> myInputState;			//Updated by UpdateEvents

		static POINT myCurrentMousePosition;				//Updated by UpdateInput
		static POINT myPreviousMousePosition;				//Updated by UpdateInput
		static POINT myInputMousePosition;					//Updated by UpdateEvents
		static POINT myMouseDelta;							//Updated by UpdateInput		

		static POINT myInputScrollWheelDelta;				//Updated by UpdateEvents
		static POINT myScrollWheelDelta;					//Updated by UpdateInput

		static HWND myWindowHandle;
	};
}


