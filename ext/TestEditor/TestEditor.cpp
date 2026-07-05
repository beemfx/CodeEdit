// (c) Beem Media. All rights reserved.

#include "ILexer.h"
#include "Lexilla.h"
#include "SciLexer.h"
#include "Scintilla.h"
#include <memory>
#include <Windows.h>

static const wchar_t TestEditor_CodeEditLibrary[] =
#if defined(_WIN64)
L"EGCodeEdit_x64.dll"
#else
L"EGCodeEdit_x86.dll"
#endif
;

class TestEditorWindowData
{
public:
	TestEditorWindowData(HWND InParentWnd)
		: m_ParentWnd(InParentWnd)
	{
		InitCodeEdit();

		m_ScintillaWnd = CreateWindowExW(
			0,
			L"Scintilla",
			L"",
			WS_CHILDWINDOW|WS_VISIBLE,
			0, 0,
			CW_USEDEFAULT, CW_USEDEFAULT,
			m_ParentWnd,
			NULL,
			NULL,
			NULL);

		if (m_ScintillaWnd)
		{
			HDC AppDC = GetDC(NULL);
			const float SystemDpiScale = (static_cast<float>(GetDeviceCaps(AppDC, LOGPIXELSX)) / USER_DEFAULT_SCREEN_DPI);

			Scintilla::ILexer5* Lexer = m_CreateLexerFn ? m_CreateLexerFn("cpp") : nullptr;
			if (Lexer)
			{
				SendMessageW(m_ScintillaWnd, SCI_SETILEXER, 0, reinterpret_cast<LPARAM>(Lexer));
			}

			const char* FontName = "Lucida Sans Typewriter";
			const int RawFontSize = 30;
			const int FontSize = static_cast<int>(RawFontSize * (1.f / SystemDpiScale) * (72.f / USER_DEFAULT_SCREEN_DPI));
			SendMessageW(m_ScintillaWnd, SCI_STYLESETFONT, STYLE_DEFAULT, reinterpret_cast<LPARAM>(const_cast<const char*>(FontName)));
			SendMessageW(m_ScintillaWnd, SCI_STYLESETSIZE, STYLE_DEFAULT, FontSize);
			SendMessageW(m_ScintillaWnd, SCI_SETMARGINS, 1, 0);
			SendMessageW(m_ScintillaWnd, SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
			SendMessageW(m_ScintillaWnd, SCI_SETMARGINWIDTHN, 0, static_cast<LPARAM>(50 * SystemDpiScale));
			SendMessageW(m_ScintillaWnd, SCI_SETTABWIDTH, 3, 0);
			SendMessageW(m_ScintillaWnd, SCI_SETUSETABS, 1, 0);

			if (true)
			{
				const LPARAM FgColor = RGB(189, 183, 183);
				const LPARAM BgColor = RGB(30, 30, 30);
				const LPARAM CommentColor = RGB(86, 177, 64);
				const LPARAM NumberColor = RGB(255, 0, 0);
				const LPARAM KeywordColor1 = RGB(255, 215, 0);
				const LPARAM KeywordColor2 = RGB(43, 145, 175);
				const LPARAM StringColor = RGB(255, 72, 255);
				const LPARAM IdentifierColor = RGB(189, 183, 107);
				const LPARAM OperatorColor = FgColor;
				const LPARAM MarginBgColor = RGB(0, 0, 0);
				const LPARAM LineNumberColor = RGB(43, 145, 175);

				SendMessageW(m_ScintillaWnd, SCI_SETCARETFORE, FgColor, 0);

				SendMessageW(m_ScintillaWnd, SCI_STYLESETFORE, STYLE_DEFAULT, FgColor);
				SendMessageW(m_ScintillaWnd, SCI_STYLESETBACK, STYLE_DEFAULT, BgColor);

				SendMessageW(m_ScintillaWnd, SCI_STYLECLEARALL, 0, 0);

				SendMessageW(m_ScintillaWnd, SCI_STYLESETFORE, SCE_C_COMMENT, CommentColor);
				SendMessageW(m_ScintillaWnd, SCI_STYLESETFORE, SCE_C_COMMENTLINE, CommentColor);
				SendMessageW(m_ScintillaWnd, SCI_STYLESETFORE, SCE_C_NUMBER, NumberColor);
				SendMessageW(m_ScintillaWnd, SCI_STYLESETFORE, SCE_C_WORD, KeywordColor1);
				SendMessageW(m_ScintillaWnd, SCI_STYLESETFORE, SCE_C_WORD2, KeywordColor2);
				SendMessageW(m_ScintillaWnd, SCI_STYLESETFORE, SCE_C_STRING, StringColor);
				SendMessageW(m_ScintillaWnd, SCI_STYLESETFORE, SCE_C_CHARACTER, StringColor);
				SendMessageW(m_ScintillaWnd, SCI_STYLESETFORE, SCE_C_IDENTIFIER, IdentifierColor);
				SendMessageW(m_ScintillaWnd, SCI_STYLESETFORE, SCE_C_OPERATOR, OperatorColor);

				SendMessageW(m_ScintillaWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, MarginBgColor);
				SendMessageW(m_ScintillaWnd, SCI_STYLESETFORE, STYLE_LINENUMBER, LineNumberColor);

				SendMessageW(m_ScintillaWnd, SCI_STYLESETSIZE, STYLE_LINENUMBER, FontSize);

				const char* Keywords1 = "void\nint\nfloat\ndouble\nshort\n";
				SendMessageW(m_ScintillaWnd, SCI_SETKEYWORDS, 0, reinterpret_cast<LPARAM>(Keywords1));
				const char* Keywords2 = "true\nfalse\nif\nelse\nthis\nreturn\nstruct\nclass\ncase\nbreak\nswitch\ncontinue\nwhile\ndo\nenum\nfor\ngoto\n";
				SendMessageW(m_ScintillaWnd, SCI_SETKEYWORDS, 1, reinterpret_cast<LPARAM>(Keywords2));
			}

			const char* SampleText =
			"// Sample Program\r\n"
			"\r\n"
			"int main()\r\n"
			"{\r\n"
			"\tprintf(\"Hello, World!\");\r\n"
			"\treturn 0;\r\n"
			"}\r\n";

			SendMessageW(m_ScintillaWnd, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(SampleText));
			SetFocus(m_ScintillaWnd);
		}
	}

	~TestEditorWindowData()
	{
		if (m_ScintillaWnd)
		{
			DestroyWindow(m_ScintillaWnd);
		}

		DeinitCodeEdit();
	}

	void HandleParentResized(UINT InWidth, UINT InHeight)
	{
		if (m_ScintillaWnd)
		{
			SetWindowPos(m_ScintillaWnd, NULL, 0, 0, InWidth, InHeight, SWP_NOMOVE|SWP_NOZORDER);
		}
	}

private:
	HWND m_ParentWnd = NULL;
	HWND m_ScintillaWnd = NULL;

	HMODULE m_CodeEditModule = NULL;

	Lexilla::GetLexerCountFn m_GetLexerCountFn = nullptr;
	Lexilla::GetLexerNameFn m_GetLexerNameFn = nullptr;
	Lexilla::GetLexerFactoryFn m_GetLexerFactoryFn = nullptr;
	Lexilla::CreateLexerFn m_CreateLexerFn = nullptr;
	Lexilla::GetLibraryPropertyNamesFn m_GetLibraryPropertyNamesFn = nullptr;
	Lexilla::SetLibraryPropertyFn m_SetLibraryPropertyFn = nullptr;
	Lexilla::GetNameSpaceFn m_GetNameSpaceFn = nullptr;

private:
	void InitCodeEdit()
	{
		m_CodeEditModule = LoadLibraryW(TestEditor_CodeEditLibrary);
		if (m_CodeEditModule)
		{
			m_GetLexerCountFn = reinterpret_cast<Lexilla::GetLexerCountFn>(GetProcAddress(m_CodeEditModule, LEXILLA_GETLEXERCOUNT));
			m_GetLexerNameFn = reinterpret_cast<Lexilla::GetLexerNameFn>(GetProcAddress(m_CodeEditModule, LEXILLA_GETLEXERNAME));
			m_GetLexerFactoryFn = reinterpret_cast<Lexilla::GetLexerFactoryFn>(GetProcAddress(m_CodeEditModule, LEXILLA_GETLEXERFACTORY));
			m_CreateLexerFn = reinterpret_cast<Lexilla::CreateLexerFn>(GetProcAddress(m_CodeEditModule, LEXILLA_CREATELEXER));
			m_GetLibraryPropertyNamesFn = reinterpret_cast<Lexilla::GetLibraryPropertyNamesFn>(GetProcAddress(m_CodeEditModule, LEXILLA_GETLIBRARYPROPERTYNAMES));
			m_SetLibraryPropertyFn = reinterpret_cast<Lexilla::SetLibraryPropertyFn>(GetProcAddress(m_CodeEditModule, LEXILLA_SETLIBRARYPROPERTY));
			m_GetNameSpaceFn = reinterpret_cast<Lexilla::GetNameSpaceFn>(GetProcAddress(m_CodeEditModule, LEXILLA_GETNAMESPACE));
		}
	}

	void DeinitCodeEdit()
	{
		m_GetLexerCountFn = nullptr;
		m_GetLexerNameFn = nullptr;
		m_GetLexerFactoryFn = nullptr;
		m_CreateLexerFn = nullptr;
		m_GetLibraryPropertyNamesFn = nullptr;
		m_SetLibraryPropertyFn = nullptr;
		m_GetNameSpaceFn = nullptr;

		if (m_CodeEditModule)
		{
			FreeLibrary(m_CodeEditModule);
			m_CodeEditModule = NULL;
		}
	}
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	const wchar_t WND_CLASS_NAME[] = L"TestEditorMainWindowClass";
	WNDCLASSW wc = { };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = WND_CLASS_NAME;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

	if (!RegisterClassW(&wc))
	{
		return 0;
	}

	HWND hwnd = CreateWindowExW(
		0, WND_CLASS_NAME, L"Test Editor", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	if (!hwnd) return 0;

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyWindow(hwnd);

	UnregisterClassW(WND_CLASS_NAME, hInstance);

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto GetWndData = [hwnd]() -> TestEditorWindowData*
		{
			return reinterpret_cast<TestEditorWindowData*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
		};

	switch (msg)
	{
	case WM_CREATE:
	{
		TestEditorWindowData* WindowData = new TestEditorWindowData(hwnd);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(WindowData));
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_SIZE:
	{
		if (TestEditorWindowData* WindowData = GetWndData())
		{
			WindowData->HandleParentResized(LOWORD(lParam), HIWORD(lParam));
		}

		return 0;
	}
	case WM_DESTROY:
	{
		TestEditorWindowData* WindowData = GetWndData();
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, NULL);
		delete WindowData;
		return 0;
	}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}
