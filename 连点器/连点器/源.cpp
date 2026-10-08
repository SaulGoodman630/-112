#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<atomic>
#include<thread>

std::atomic <bool> isRunning(false);
std::thread clickThread;
HINSTANCE g_hinst;

void ClickLoop() {
	while (isRunning) {
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

		Sleep(100);
	}
}

#define HOTKEY_START 1//F8开启
#define HOTKEY_STOP 2//F9停止

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM IParam)
{
	switch (message)
	{
		case WM_CREATE: //创建时注册热键
			RegisterHotKey(hWnd, HOTKEY_START, 0, VK_F8);
			RegisterHotKey(hWnd, HOTKEY_STOP, 0, VK_F9);
			return 0;

		case WM_HOTKEY://捕获到热键按下
			if (wParam == HOTKEY_START && !isRunning) {
				isRunning.store(true);
				clickThread = std::thread(ClickLoop);
			}
			else if (wParam == HOTKEY_STOP && isRunning) {
				isRunning.store(false);
				if (clickThread.joinable()) clickThread.join();
			}
			return 0;

		case WM_DESTROY:
			UnregisterHotKey(hWnd, HOTKEY_START);
			UnregisterHotKey(hWnd, HOTKEY_STOP);
			if (isRunning) {
				isRunning.store(false);
				if (clickThread.joinable()) clickThread.join();
			}
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, message, wParam, IParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR IpCmdLine, int nCmdShow)
{
	WNDCLASS wc = {0};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"MywindowsClass";

	if (!RegisterClass(&wc)) {
		MessageBox(NULL, L"创建窗口失败", L"错误", MB_OK);
		return 1;
	}
	g_hinst = hInstance;

	HWND hWnd = CreateWindowEx(
		0,
		wc.lpszClassName,//窗口类名
		L"自动连点器",//窗口标题
		WS_OVERLAPPEDWINDOW,//窗口样式
		CW_USEDEFAULT, CW_USEDEFAULT,//位置与大小
		300, 200,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd) {
		MessageBox(NULL, L"创建窗口失败", L"错误", MB_OK);
		return 1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return(int)msg.wParam;
}