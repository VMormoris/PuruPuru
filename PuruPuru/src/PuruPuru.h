#pragma once
#include <application.h>

struct PuruPuru : public Application {
    static PuruPuru* sInstance;

    PuruPuru(const char* name)
        : Application(name) { sInstance = this; }
    void OnStart(void) override;
    void OnStop(void) override;

    void OnFrame(float deltaTime) override;

private:

    bool IsMaximized(void) const;
    bool IsTitlebarHovered(void) const { return mTitlebarHovered; }

    void DrawMainWindow(void);
    void DrawThemeWindow(void);
    void DrawCustomTitleBar(float& outTitlebarHeight);
    void DrawMenubar(void);

private:
    bool mTitlebarHovered = false;
};
