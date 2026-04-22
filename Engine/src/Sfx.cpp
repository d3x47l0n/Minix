#include "Sfx.h"

namespace Engine
{
    std::vector<Sfx::Entry> Sfx::_entries;
    bool Sfx::_active = false;

    bool Sfx::Init()
    {
        if (_active) return true;

        _entries.clear();

        _entries.push_back({ "pwii", "audio/pwii.wav", {}, false });
        _entries.push_back({ "pwuu", "audio/pwuu.wav", {}, false });

        for (auto& entry : _entries)
        {
            entry.sound = LoadSound(entry.path);
            if (entry.sound.frameCount > 0)
            {
                entry.loaded = true;
            }
            else
            {
                entry.loaded = false;
                TraceLog(LOG_WARNING, "Sfx::Init: failed to load %s", entry.path);
            }
        }

        _active = true;
        return true;
    }

    void Sfx::Stop()
    {
        if (!_active) return;

        for (auto& entry : _entries)
        {
            if (entry.loaded)
            {
                UnloadSound(entry.sound);
                entry.sound = Sound{};
                entry.loaded = false;
            }
        }

        _entries.clear();
        _active = false;
    }

    int Sfx::GetCount()
    {
        return static_cast<int>(_entries.size());
    }

    const char* Sfx::GetName(int id)
    {
        if (id < 0 || id >= static_cast<int>(_entries.size()))
            return nullptr;

        return _entries[id].name;
    }

    Sound* Sfx::Get(int id)
    {
        if (id < 0 || id >= static_cast<int>(_entries.size()))
            return nullptr;

        if (!_entries[id].loaded)
            return nullptr;

        return &_entries[id].sound;
    }

    bool Sfx::IsValid(int id)
    {
        return Get(id) != nullptr;
    }

    void Sfx::Play(int id)
    {
        Sound* snd = Get(id);
        if (!snd) return;

        PlaySound(*snd);
    }
}