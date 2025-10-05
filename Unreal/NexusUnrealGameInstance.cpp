#include "NexusUnrealGameInstance.h"
#include "Framework/Application/SlateApplication.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/HideWindowsPlatformTypes.h"
#endif

void UNexusUnrealGameInstance::Init()
{
    Super::Init();

#if PLATFORM_WINDOWS
    // 获取窗口
    TSharedPtr<SWindow> SlateWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
    if (!SlateWindow.IsValid()) return;

    HWND hWnd = (HWND)SlateWindow->GetNativeWindow()->GetOSWindowHandle();

    // 设置窗口样式：不显示任务栏
    LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
    exStyle &= ~WS_EX_APPWINDOW;
    exStyle |= WS_EX_TOOLWINDOW;
    SetWindowLong(hWnd, GWL_EXSTYLE, exStyle);

    // 初始化托盘数据
    ZeroMemory(&TrayIconData, sizeof(NOTIFYICONDATA));
    TrayIconData.cbSize = sizeof(NOTIFYICONDATA);
    TrayIconData.hWnd = hWnd;
    TrayIconData.uID = 1;
    TrayIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    TrayIconData.uCallbackMessage = WM_USER + 2000;

    // 图标路径
    //FString IconPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Binaries/Win64/icon.ico"));
    FString IconPath = FPaths::ProjectContentDir() / TEXT("Icons/icon.ico");

    HICON hIcon = (HICON)LoadImageW(
        NULL,
        *IconPath,
        IMAGE_ICON,
        32,
        32,
        LR_LOADFROMFILE
    );


    TrayIconData.hIcon = hIcon;

    wcscpy_s(TrayIconData.szTip, L"NexusUnreal Runing...");

    Shell_NotifyIcon(NIM_ADD, &TrayIconData);

    // 注册消息处理器
    if (FSlateApplication::IsInitialized())
    {
        FWindowsApplication* App = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();
        if (App)
        {
            App->AddMessageHandler(*this);
        }
    }
#endif
}

void UNexusUnrealGameInstance::Shutdown()
{
    Super::Shutdown();

#if PLATFORM_WINDOWS
    Shell_NotifyIcon(NIM_DELETE, &TrayIconData);

    if (FSlateApplication::IsInitialized())
    {
        FWindowsApplication* App = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();
        if (App)
        {
            App->RemoveMessageHandler(*this);
        }
    }
#endif
}

#if PLATFORM_WINDOWS
void UNexusUnrealGameInstance::AddTrayMenu(HWND hwnd)
{
    POINT pt;
    GetCursorPos(&pt);
    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_STRING, WM_DESTROY, L"Exit NexusUnreal");

    SetForegroundWindow(hwnd);
    int32 MenuID = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

    if (MenuID == WM_DESTROY)
    {
        RequestEngineExit(TEXT("Tray Quit"));
    }

    DestroyMenu(menu);
}

bool UNexusUnrealGameInstance::ProcessMessage(HWND hwnd, uint32 msg, WPARAM wParam, LPARAM lParam, int32& outResult)
{
    if (msg == WM_USER + 2000) // 托盘消息
    {
        if (lParam == WM_RBUTTONUP) // 右键松开
        {
            AddTrayMenu(hwnd);
            return true;
        }
    }
    return false;
}
#endif
