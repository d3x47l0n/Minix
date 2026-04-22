#include "Gfx.h"

namespace Engine
{
    std::vector<Gfx::Entry> Gfx::_entries;
    bool Gfx::_active = false;

    bool Gfx::Init()
    {
        if (_active) return true;

        _entries.clear();

        _entries.push_back({ "diamantBlue", "graphics/diamondBlue.png", {}, false });
        _entries.push_back({ "diamantYellow", "graphics/diamondYellow.png", {}, false });
        _entries.push_back({ "diamantOrange", "graphics/diamondOrange.png", {}, false });
        _entries.push_back({ "diamantGreen", "graphics/diamondGreen.png", {}, false });

        _entries.push_back({ "rombBlue", "graphics/rombBlue.png", {}, false });
        _entries.push_back({ "rombYellow", "graphics/rombYellow.png", {}, false });
        _entries.push_back({ "rombOrange", "graphics/rombOrange.png", {}, false });
        _entries.push_back({ "rombGreen", "graphics/rombGreen.png", {}, false });

        _entries.push_back({ "keyBlue", "graphics/keyBlue.png", {}, false });
        _entries.push_back({ "keyYellow", "graphics/keyYellow.png", {}, false });
        _entries.push_back({ "keyOrange", "graphics/keyOrange.png", {}, false });
        _entries.push_back({ "keyGreen", "graphics/keyGreen.png", {}, false });

        _entries.push_back({ "heart", "graphics/heart.png", {}, false });
        _entries.push_back({ "heartHalf", "graphics/heartHalf.png", {}, false });

        for (auto& entry : _entries)
        {
            entry.texture = LoadTexture(entry.path);
            if (entry.texture.id != 0)
            {
                entry.loaded = true;
            }
            else
            {
                entry.loaded = false;
                TraceLog(LOG_WARNING, "Gfx::Init: failed to load %s", entry.path);
            }
        }

        _active = true;
        return true;
    }

    void Gfx::Stop()
    {
        if (!_active) return;

        for (auto& entry : _entries)
        {
            if (entry.loaded && entry.texture.id != 0)
            {
                UnloadTexture(entry.texture);
                entry.texture = Texture2D{};
                entry.loaded = false;
            }
        }

        _entries.clear();
        _active = false;
    }

    int Gfx::GetCount()
    {
        return static_cast<int>(_entries.size());
    }

    const char* Gfx::GetName(int id)
    {
        if (id < 0 || id >= static_cast<int>(_entries.size()))
            return nullptr;

        return _entries[id].name;
    }

    Texture2D* Gfx::Get(int id)
    {
        if (id < 0 || id >= static_cast<int>(_entries.size()))
            return nullptr;

        if (!_entries[id].loaded)
            return nullptr;

        return &_entries[id].texture;
    }

    bool Gfx::IsValid(int id)
    {
        return Get(id) != nullptr;
    }
}