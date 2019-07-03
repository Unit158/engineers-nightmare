#include <map>
#include <stack>
#include <string>
#include <vector>
#include "..\asset_manager.h"
#include "sound.h"
#include "soloud.h"
#include "soloud_wav.h"

audio_engine::audio_engine(asset_manager & assetman)
{
    engine = SoLoud::Soloud();
    engine.init();

    world_bus = SoLoud::Bus();
    player_bus = SoLoud::Bus();
    menu_bus = SoLoud::Bus();
    music_bus = SoLoud::Bus();
    atmosphere_filter = SoLoud::BiquadResonantFilter();
    world_bus.setFilter(0, &atmosphere_filter);

    world_bus_handle = engine.play(world_bus);
    player_bus_handle = engine.play(player_bus);
    menu_bus_handle = engine.play(menu_bus);
    music_bus_handle = engine.play(music_bus);
    engine.play(menu_bus);

    music.load();
    engine.play(music);
}

audio_engine::~audio_engine()
{
    engine.deinit();
}

void audio_engine::tick()
{
}

void audio_engine::apply_atmosphere_filter(float pressure)
{
    atmosphere_filter.setParams(SoLoud::BiquadResonantFilter::LOWPASS, 1, 22000 * pressure * pressure, 0);
}

void audio_engine::play_world_sound(SoLoud::AudioSource & sound)
{
    world_bus.play(sound);
}

void audio_engine::play_player_sound(SoLoud::AudioSource & sound)
{
    player_bus.play(sound);
}


void audio_engine::play_menu_sound(SoLoud::AudioSource & sound)
{
    menu_bus.play(sound);
}

void audio_engine::update_menu_volume(float volume, bool kill)
{
    if (volume < 1 / 128)
        return;

    menu_bus.setVolume(volume);
    menu_bus.setInaudibleBehavior(true, kill);
}

void audio_engine::update_music_volume(float volume, bool kill)
{
    if (volume < 1 / 128)
        return;

    music_bus.setVolume(volume);
    music_bus.setInaudibleBehavior(true, kill);
}

void audio_engine::update_world_player_volume(float volume, bool kill)
{
    if (volume < 1 / 128)
        return;

    world_bus.setVolume(volume);
    world_bus.setInaudibleBehavior(true, kill);
    player_bus.setVolume(volume);
    player_bus.setInaudibleBehavior(true, kill);
}

void audio_engine::update_music_mood(std::vector<std::pair<std::string, float>> moods)
{
    ;
}

void audio_engine::pause_world_player()
{
    for (auto handle : world_handles)
    {
        engine.setPause(handle, true);
    }

    for (auto handle : player_handles)
    {
        engine.setPause(handle, true);
    }
}

void audio_engine::play_world_player()
{
    for (auto handle : world_handles)
    {
        engine.setPause(handle, false);
    }

    for (auto handle : player_handles)
    {
        engine.setPause(handle, false);
    }
}

void audio_engine::next_music_cue()
{
    
}