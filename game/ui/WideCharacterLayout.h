#pragma once

#include "game/ui/GameplayOverlayTypes.h"

#include <algorithm>
#include <optional>

namespace OpenYAMM::Game
{
// Character pages keep their native 640-wide coordinates; only their placement and uniform scale change.
struct WideCharacterLayout
{
    float scale = 1;
    float contentX = 0;
    float contentY = 0;
    float portraitScale = 1;
    float portraitX = 0;
    float portraitY = 0;
    float screenWidth = 0;
    float screenHeight = 0;

    static std::optional<WideCharacterLayout> compute(int width, int height, size_t memberCount)
    {
        if (height <= 0 || width < height * 1.65f || memberCount == 0 || memberCount > 5)
        {
            return std::nullopt;
        }
        WideCharacterLayout layout;
        layout.screenWidth = float(width);
        layout.screenHeight = float(height);
        layout.scale = std::min({width / 764.0f, height / 436.0f, 8.0f});
        const float columnWidth = 100 * layout.scale;
        layout.contentX = columnWidth + 12 * layout.scale;
        layout.contentY = (height - 26 * layout.scale - 410 * layout.scale) * 0.5f;
        layout.portraitScale = std::min(layout.scale,
            (height - 26 * layout.scale) / (memberCount * 90.0f + 16));
        layout.portraitX = (columnWidth - 78 * layout.portraitScale) * 0.5f;
        layout.portraitY = (height - 26 * layout.scale - memberCount * 90 * layout.portraitScale) * 0.5f
            + 7 * layout.portraitScale;
        return layout;
    }

    GameplayResolvedHudLayoutElement contentRect() const
    {
        return {contentX, contentY, screenWidth - contentX - 12 * scale, 410 * scale, scale};
    }

    float extraContentWidth() const
    {
        return std::max(0.0f, contentRect().width / scale - 640);
    }

    GameplayResolvedHudLayoutElement frameRect() const
    {
        return {0, 0, screenWidth, screenHeight, scale};
    }

    GameplayResolvedHudLayoutElement portraitPanelRect() const
    {
        return {0, 0, 100 * scale, screenHeight - 26 * scale, scale};
    }

    GameplayResolvedHudLayoutElement statusRect() const
    {
        return {6 * scale, screenHeight - 23 * scale, screenWidth - 12 * scale, 17 * scale, scale};
    }

    GameplayResolvedHudLayoutElement portraitSlot(size_t index) const
    {
        return {portraitX, portraitY + index * 90 * portraitScale,
            78 * portraitScale, 81 * portraitScale, portraitScale};
    }

    GameplayResolvedHudLayoutElement placeContent(GameplayResolvedHudLayoutElement rect) const
    {
        return {contentX + rect.x * scale, contentY + rect.y * scale,
            rect.width * scale, rect.height * scale, rect.scale * scale};
    }

    GameplayResolvedHudLayoutElement placePortrait(const GameplayResolvedHudLayoutElement &rect,
        const GameplayResolvedHudLayoutElement &nativeSlot, size_t index) const
    {
        const GameplayResolvedHudLayoutElement slot = portraitSlot(index);
        return {slot.x + (rect.x - nativeSlot.x) * slot.scale,
            slot.y + (rect.y - nativeSlot.y) * slot.scale,
            rect.width * slot.scale, rect.height * slot.scale, rect.scale * slot.scale};
    }
};
}
