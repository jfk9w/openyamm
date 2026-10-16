#include "doctest/doctest.h"

#include "game/ui/WideCharacterLayout.h"

using namespace OpenYAMM::Game;

TEST_CASE("wide character layout fits all party sizes without stretching pages or covering the status rail")
{
    for (const std::pair<int, int> size : {std::pair{1280, 720}, {1920, 1080}, {2400, 1080}, {2560, 1080}})
    {
        for (size_t count = 1; count <= 5; ++count)
        {
            const std::optional<WideCharacterLayout> layout =
                WideCharacterLayout::compute(size.first, size.second, count);
            REQUIRE(layout);
            const GameplayResolvedHudLayoutElement content = layout->contentRect();
            CHECK(content.x >= 100 * layout->scale);
            CHECK(content.y >= -0.001f);
            CHECK(content.x + content.width <= size.first);
            CHECK(content.x + content.width == doctest::Approx(size.first - 12 * layout->scale));
            CHECK(content.y + content.height <= doctest::Approx(size.second - 26 * layout->scale));
            const GameplayResolvedHudLayoutElement grid = layout->placeContent({7, 32, 448, 288, 1});
            CHECK(grid.width / grid.height == doctest::Approx(448.0f / 288));
            CHECK(grid.width / 14 == doctest::Approx(grid.height / 9));
            CHECK(grid.scale == content.scale);
            for (size_t index = 0; index < count; ++index)
            {
                const GameplayResolvedHudLayoutElement slot = layout->portraitSlot(index);
                CHECK(slot.x >= 0);
                CHECK(slot.y - 5.5f * slot.scale >= 0);
                CHECK(slot.x + slot.width < content.x);
                CHECK(slot.y + slot.height < size.second - 26 * layout->scale);
                if (index > 0)
                {
                    const GameplayResolvedHudLayoutElement previous = layout->portraitSlot(index - 1);
                    CHECK(slot.x == previous.x);
                    CHECK(slot.y > previous.y + previous.height);
                }
            }
        }
    }
}

TEST_CASE("wide character portraits retain their face, arcs and indicators inside each vertical slot")
{
    const WideCharacterLayout layout = *WideCharacterLayout::compute(2400, 1080, 5);
    for (size_t index = 0; index < 5; ++index)
    {
        const GameplayResolvedHudLayoutElement nativeSlot = {19 + index * 94.0f, 394, 78, 81, 1};
        const GameplayResolvedHudLayoutElement nativeFace =
            {nativeSlot.x + 16.333333f, nativeSlot.y + 8, 45.333333f, 60.266667f, 1};
        const GameplayResolvedHudLayoutElement face = layout.placePortrait(nativeFace, nativeSlot, index);
        const GameplayResolvedHudLayoutElement slot = layout.portraitSlot(index);
        CHECK(face.width / face.height == doctest::Approx(nativeFace.width / nativeFace.height));
        CHECK(face.x > slot.x);
        CHECK(face.y > slot.y);
        CHECK(face.x + face.width < slot.x + slot.width);
        CHECK(face.y + face.height < slot.y + slot.height);
    }
}

TEST_CASE("wide character layout leaves narrow and invalid viewports to the original layout")
{
    CHECK_FALSE(WideCharacterLayout::compute(640, 480, 4));
    CHECK_FALSE(WideCharacterLayout::compute(720, 1280, 4));
    CHECK_FALSE(WideCharacterLayout::compute(0, 0, 4));
    CHECK_FALSE(WideCharacterLayout::compute(1920, 1080, 0));
    CHECK_FALSE(WideCharacterLayout::compute(1920, 1080, 6));
}
