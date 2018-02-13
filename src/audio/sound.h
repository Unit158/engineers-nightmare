#pragma once
#include <map>

#include "soloud.h"
#include "soloud_biquadresonantfilter.h"

class audio_engine
{
    std::map<SoLoud::handle, void (&)()> callbacks;
    std::vector<SoLoud::handle> world_handles;
    std::vector<SoLoud::handle> player_handles;
    std::vector<SoLoud::handle> menu_handles;
    std::vector<SoLoud::handle> music_handles;
    SoLoud::Soloud engine;
    SoLoud::handle world_bus_handle;
    SoLoud::handle player_bus_handle;
    SoLoud::handle menu_bus_handle;
    SoLoud::handle music_bus_handle;
    SoLoud::Bus world_bus;
    SoLoud::Bus player_bus;
    SoLoud::Bus menu_bus;
    SoLoud::Bus music_bus;
    SoLoud::BiquadResonantFilter atmosphere_filter;

public:
    audio_engine();
    ~audio_engine();

    void play_world_sound(SoLoud::AudioSource & sound);
    void play_world_sound(SoLoud::AudioSource & sound, void (&on_finish) ()); /* this should probably die */
    void play_player_sound(SoLoud::AudioSource & sound);
    void play_player_sound(SoLoud::AudioSource & sound, void (&on_finish) ());
    void play_menu_sound(SoLoud::AudioSource & sound);
    void play_menu_sound(SoLoud::AudioSource & sound, void (&on_finish) ());

    void apply_atmosphere_filter(float pressure);
    void update_music_mood(std::vector<std::pair<std::string, float>> moods);
    void update_world_player_volume(float volume, bool kill = false);
    void update_menu_volume(float volume, bool kill = false);
    void update_music_volume(float volume, bool kill = false);
    void pause_world_player();
    void play_world_player();

    void tick();
};