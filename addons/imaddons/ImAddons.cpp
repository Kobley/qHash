/*
This is a simple items addons for Dear ImGui
*/

#include "ImAddons.h"

void TextCentered(std::string text) {
    float win_width = ImGui::GetWindowSize().x;
    float text_width = ImGui::CalcTextSize(text.c_str()).x;
    float text_indentation = (win_width - text_width) * 0.5f;

    float min_indentation = 20.0f;
    if (text_indentation <= min_indentation) {
        text_indentation = min_indentation;
    }

    ImGui::SameLine(text_indentation);
    ImGui::PushTextWrapPos(win_width - text_indentation);
    ImGui::TextWrapped(text.c_str());
    ImGui::PopTextWrapPos();
}

void AddUnderLine(ImColor col_)
{
    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    min.y = max.y;
    ImGui::GetWindowDrawList()->AddLine(min, max, col_, 1.0f);
}

bool ImAdd::ProgressBarBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = size_arg;
    size.x -= style.FramePadding.x * 2;

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    // Render
    const float circleStart = size.x * 0.7f;
    const float circleEnd = size.x;
    const float circleWidth = circleEnd - circleStart;

    window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
    window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

    /*
    const float t = g.Time;
    const float r = size.y / 2;
    const float speed = 1.5f;

    const float a = speed*0;
    const float b = speed*0.333f;
    const float c = speed*0.666f;

    const float o1 = (circleWidth+r) * (t+a - speed * (int)((t+a) / speed)) / speed;
    const float o2 = (circleWidth+r) * (t+b - speed * (int)((t+b) / speed)) / speed;
    const float o3 = (circleWidth+r) * (t+c - speed * (int)((t+c) / speed)) / speed;

    window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
    window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
    window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
    */
}

void ImAdd::NavigationRadio(const char* text, ImVec2 size, int id, int* t_ids)
{
    if (id == *t_ids)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        auto BackgroundDrawList = ImGui::GetBackgroundDrawList();
        auto ForegroundDrawList = ImGui::GetForegroundDrawList();
        auto WindowDrawList = ImGui::GetWindowDrawList();
        auto WindowSize = ImGui::GetWindowSize();
        auto WindowPos = ImGui::GetWindowPos();
        auto CursorPos = ImGui::GetCursorPos();

        static int size_line = 2;
        static int style_line = 0;

        if (style_line == 0)
            ForegroundDrawList->AddRectFilled(ImVec2(CursorPos.x, (size.y + style.WindowPadding.y*2) + CursorPos.y + style.WindowPadding.y + style.ItemSpacing.y), ImVec2(CursorPos.x + size_line, (size.y + style.WindowPadding.y*2) + CursorPos.y + size.y), IM_COL32(204, 204, 204, 255), 1, ImDrawFlags_RoundCornersRight);
        else if (style_line == 1)
            ForegroundDrawList->AddRectFilled(ImVec2(CursorPos.x + style.WindowPadding.x + style.ItemSpacing.x, CursorPos.y + style.WindowPadding.y + style.ItemSpacing.y), ImVec2(CursorPos.x + size_line + style.WindowPadding.x + style.ItemSpacing.x, CursorPos.y + size.y), IM_COL32(204, 204, 204, 255), 6);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.21f, 0.21f, 0.21f, 0.75f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        ImGui::Button(text, size);
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.34f, 0.34f, 0.35f, 0.50f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.21f, 0.21f, 0.21f, 0.75f));
        if (ImGui::Button(text, size))
        {
            *t_ids = id;
        }
        ImGui::PopStyleColor(3);
    }
}

// hyperlink urls
void ImAdd::TextURL(const char* name_, const char* popup, LPCWSTR URL_, bool underlined)
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.00f, 0.60f, 1.00f, 1.00f));
    ImGui::Text(name_);
    ImGui::PopStyleColor();
    if (ImGui::IsItemHovered())
    {
        if (ImGui::IsMouseClicked(0))
        {
            ShellExecuteW(NULL, L"open", URL_, nullptr, nullptr, SW_HIDE);
        }
        ImGui::SetTooltip(popup);
    }
    /*
    else
    {
        AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_Button]);
    }
    */
    if (underlined)
        AddUnderLine(ImVec4(0.00f, 0.45f, 1.00f, 1.00f));
}