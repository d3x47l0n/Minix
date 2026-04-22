#pragma once

#include <vector>

#include "raylib.h"

namespace Engine
{
    class Sfx
    {
    public:
        static bool Init();
        static void Stop();

        static int GetCount();
        static const char* GetName(int id);
        static Sound* Get(int id);
        static bool IsValid(int id);

        static void Play(int id);

    private:
        struct Entry
        {
            const char* name = nullptr;
            const char* path = nullptr;
            Sound sound{};
            bool loaded = false;
        };

        static std::vector<Entry> _entries;
        static bool _active;
    };
}