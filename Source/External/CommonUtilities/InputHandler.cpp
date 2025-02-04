#include "pch.h"
#include "InputHandler.h"




std::array<bool, 256> CommonUtilities::InputHandler::myCurrentState;
std::array<bool, 256> CommonUtilities::InputHandler::myPreviousState;
std::array<bool, 256> CommonUtilities::InputHandler::myInputState;

POINT CommonUtilities::InputHandler::myCurrentMousePosition;				//Updated by UpdateInput
POINT CommonUtilities::InputHandler::myPreviousMousePosition;				//Updated by UpdateInput
POINT CommonUtilities::InputHandler::myInputMousePosition;					//Updated by UpdateEvents
POINT CommonUtilities::InputHandler::myMouseDelta;							//Updated by UpdateInput		

POINT CommonUtilities::InputHandler::myInputScrollWheelDelta;				//Updated by UpdateEvents
POINT CommonUtilities::InputHandler::myScrollWheelDelta;					//Updated by UpdateInput

HWND CommonUtilities::InputHandler::myWindowHandle;




bool CommonUtilities::InputHandler::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		myInputState[wParam] = true;

		return true;
		break;

	case WM_KEYUP:
		myInputState[wParam] = false;

		return true;
		break;

	case WM_MOUSEMOVE:
		myInputMousePosition.x = GET_X_LPARAM(lParam);
		myInputMousePosition.y = GET_Y_LPARAM(lParam);

		return true;
		break;



		//Mouse buttons
	case WM_LBUTTONDOWN:
		myInputState[static_cast<int>(Keys::MOUSELBUTTON)] = true;
		return true;
		break;

	case WM_LBUTTONUP:
		myInputState[static_cast<int>(Keys::MOUSELBUTTON)] = false;
		return true;
		break;

	case WM_RBUTTONDOWN:
		myInputState[static_cast<int>(Keys::MOUSERBUTTON)] = true;
		return true;
		break;

	case WM_RBUTTONUP:
		myInputState[static_cast<int>(Keys::MOUSERBUTTON)] = false;
		return true;
		break;

	case WM_MBUTTONDOWN:
		myInputState[static_cast<int>(Keys::MBUTTON)] = true;
		return true;
		break;

	case WM_MBUTTONUP:
		myInputState[static_cast<int>(Keys::MBUTTON)] = false;
		return true;
		break;



		//Scrollwheel
	case WM_MOUSEWHEEL:
		myInputScrollWheelDelta.y += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		return true;
		break;

	case WM_MOUSEHWHEEL:
		myInputScrollWheelDelta.x += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		return true;
		break;

	default:
		break;
	}

	return false;
}

bool CommonUtilities::InputHandler::IsKeyDown(const int aKeyCode)
{
	return myCurrentState[aKeyCode] && !myPreviousState[aKeyCode];
}

bool CommonUtilities::InputHandler::IsKeyPressed(const int aKeyCode)
{
	return myCurrentState[aKeyCode];
}

bool CommonUtilities::InputHandler::IsKeyReleased(const int aKeyCode)
{
	return !myCurrentState[aKeyCode] && myPreviousState[aKeyCode];
}

bool CommonUtilities::InputHandler::IsKeyDown(const Keys aKeyCode)
{
	return myCurrentState[static_cast<int>(aKeyCode)] && !myPreviousState[static_cast<int>(aKeyCode)];
}

bool CommonUtilities::InputHandler::IsKeyPressed(const Keys aKeyCode)
{
	return myCurrentState[static_cast<int>(aKeyCode)];
}

bool CommonUtilities::InputHandler::IsKeyReleased(const Keys aKeyCode)
{
	return !myCurrentState[static_cast<int>(aKeyCode)] && myPreviousState[static_cast<int>(aKeyCode)];
}

POINT CommonUtilities::InputHandler::GetMousePosition()
{
	return myCurrentMousePosition;
}

POINT CommonUtilities::InputHandler::GetMouseDelta()
{
	return myMouseDelta;
}

POINT CommonUtilities::InputHandler::GetScrollWheelDelta()
{
	return myScrollWheelDelta;
}

bool CommonUtilities::InputHandler::SetMousePosition(const POINT aPosition)
{
	return SetCursorPos(aPosition.x, aPosition.y);
}

bool CommonUtilities::InputHandler::CaptureMouse()
{
	if (myWindowHandle == NULL)
	{
		return false;
	}

	HWND value = SetCapture(myWindowHandle);


	//Constrain cursor to window
	RECT rect;
	GetClientRect(myWindowHandle, &rect);

	POINT upperLeft;
	upperLeft.x = rect.left;
	upperLeft.y = rect.top;

	POINT lowerRight;
	lowerRight.x = rect.right;
	lowerRight.y = rect.bottom;

	MapWindowPoints(myWindowHandle, nullptr, &upperLeft, 1);
	MapWindowPoints(myWindowHandle, nullptr, &lowerRight, 1);

	rect.left = upperLeft.x;
	rect.top = upperLeft.y;

	rect.right = lowerRight.x;
	rect.bottom = lowerRight.y;

	return ClipCursor(&rect);
}

bool CommonUtilities::InputHandler::UncaptureMouse()
{

	if (ReleaseCapture())
	{
		ClipCursor(nullptr);

		return true;
	}
	else
	{
		return false;
	}
}

void CommonUtilities::InputHandler::SetWindowHandle(HWND aWindowHandle)
{
	myWindowHandle = aWindowHandle;
}

void CommonUtilities::InputHandler::UpdateInput()
{
	myPreviousState = myCurrentState;
	myCurrentState = myInputState;

	myPreviousMousePosition = myCurrentMousePosition;
	myCurrentMousePosition = myInputMousePosition;



	myMouseDelta.x = myCurrentMousePosition.x - myPreviousMousePosition.x;
	myMouseDelta.y = myCurrentMousePosition.y - myPreviousMousePosition.y;

	myScrollWheelDelta = myInputScrollWheelDelta;
	myInputScrollWheelDelta.x = 0;
	myInputScrollWheelDelta.y = 0;
}
