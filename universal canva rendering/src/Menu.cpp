#include "Menu.h"

SDK::FVector2D Menu::menuPos{ 100.0f, 100.0f };
bool Menu::bMenu{ true };


static int currentTab = 0;

void Menu::DrawMenu(SDK::UCanvas* canvas)
{
    ZeroGUI::SetupCanvas(canvas);
    ZeroGUI::hover_element = false;
    ZeroGUI::Input::Handle();

    if (!ZeroGUI::Window("unversal zerogui canva", &menuPos, SDK::FVector2D{ 500.0f, 400.0f }, bMenu))
        return;

    ZeroGUI::Text("if you see this menu, everything worked fine!");
    ZeroGUI::Text("press insert to close, press end to unload module");



    ZeroGUI::Render();
    ZeroGUI::Draw_Cursor(true);
}
