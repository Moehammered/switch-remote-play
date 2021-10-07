#ifndef __DECODERTHREADMENU_H__
#define __DECODERTHREADMENU_H__

#include "Menu.h"
#include "../decoder/DecoderOptions.h"
#include "../utils/ArrayCirculator.h"
#include "../utils/UnorderedMapCirculator.h"
#include "../utils/RangedIterator.h"
#include "../decoder/ThreadType.h"

namespace decoderUtils
{
    enum class DecoderThreadProp
    {
        Type,
        Count
    };

    static std::vector<DecoderThreadProp> const ThreadProperties
    {
        DecoderThreadProp::Type, DecoderThreadProp::Count
    };
};

class DecoderThreadMenu : public Menu
{
public:
    DecoderThreadMenu();

    void ProcessInput(PadState const & pad) override;
    void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

    void SetTypeFlag(int32_t flag);
    void SetThreadCount(int32_t count);

    int32_t TypeFlag() const;
    int32_t ThreadCount() const;

private:
    std::unordered_map<decoderUtils::DecoderThreadProp, Text> textElements;
    ArrayCirculator<decoderUtils::DecoderThreadProp, std::vector> itemCursor;
    UnorderedMapCirculator<int32_t, std::string> typeCursor;
    int32_t threadCount;

    void SetupText();
    void UpdateUI(decoderUtils::DecoderThreadProp prop);
    void ShiftParam(decoderUtils::DecoderThreadProp prop, int direction);
};

#endif
