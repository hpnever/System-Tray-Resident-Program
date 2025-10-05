#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsApplication.h"
#include "Windows/MinWindows.h"
#include <shellapi.h>
#endif

#include "NexusUnrealGameInstance.generated.h"

UCLASS()
class NEXUSUNREAL_API UNexusUnrealGameInstance : public UGameInstance
#if PLATFORM_WINDOWS
    , public IWindowsMessageHandler
#endif
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Shutdown() override;

#if PLATFORM_WINDOWS
    virtual bool ProcessMessage(HWND hwnd, uint32 msg, WPARAM wParam, LPARAM lParam, int32& outResult) override;
    void AddTrayMenu(HWND hwnd);
    NOTIFYICONDATA TrayIconData;
#endif
};
