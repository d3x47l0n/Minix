#pragma once

#include <vector>
#include <string>

#include "raylib.h"

namespace Engine
{
    class Gfx
    {
    public:
        static bool Init();
        static void Stop();

        static int GetCount();
        static const char* GetName(int id);
        static Texture2D* Get(int id);
        static bool IsValid(int id);

    private:
        struct Entry
        {
            const char* name = nullptr;
            const char* path = nullptr;
            Texture2D texture{};
            bool loaded = false;
        };

        static std::vector<Entry> _entries;
        static bool _active;
    };
}