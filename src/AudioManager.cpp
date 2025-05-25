#include "AudioManager.h"

//-----------------------------------------------------------------------------
// Returns reference to singelton instance
//-----------------------------------------------------------------------------
AudioManager& AudioManager::GetInstance()
{
    static AudioManager instance;
    return instance;
}


//-----------------------------------------------------------------------------
// Initalizes SDL_Mixer and sets up spec and audio device for 
// SDL_mixer to use
//-----------------------------------------------------------------------------
bool AudioManager::Init()
{
    // Initalizes SDL's default audio subsystem
    if (!SDL_Init(SDL_INIT_AUDIO))
    {
        std::cerr << "SDL could not initialize audio! Error: " << SDL_GetError() << '\n';
        return false;
    }

    // Opens an audio device for SDL_mixer to use
    if (!Mix_OpenAudio(0, NULL))
    {
        std::cerr << "SDL_mixer could not open an audio device! Error: " << SDL_GetError() << '\n';
        return false;
    }

    return true;
}


//-----------------------------------------------------------------------------
// Cleans up all resources used by AudioManager and exits SDL_Mixer
//-----------------------------------------------------------------------------
void AudioManager::Destroy()
{
    // Destroys all Mix_Chunks
    UnloadAllAudio();

    // Closes SDL_mixer open audio device
    Mix_CloseAudio();

    Mix_Quit();

    // Since only this class uses the SDL audio-subsystem
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}


//-----------------------------------------------------------------------------
// Creates a singel Mix_Chunk, specified by an AudioEnum,
// and adds it to m_audioChunks 
//-----------------------------------------------------------------------------
bool AudioManager::LoadAudio(AudioEnum audioID)
{
    // Reload WAV file if audio is already loaded
    Mix_Chunk*& audioChunk = m_audioChunks[static_cast<int>(audioID)];
    if (audioChunk != nullptr)
    {
        std::cout << "WAV file is already loaded, reloading." << '\n';
        Mix_FreeChunk(audioChunk);
    }
    
    // Gets filepath to audio file that is going to be loaded
    std::string filepath = "./assets/audio/" + GetAudioFilepath(audioID);

    // Attempts to load the WAV file
    audioChunk = Mix_LoadWAV(filepath.c_str());
    if (audioChunk == nullptr)
    {
        std::cerr << "Mix_LoadWAV failed! Error: " << SDL_GetError() << '\n';
        return false;
    }
    
    return true;
}


//-----------------------------------------------------------------------------
// Creates Mix_Chunks for all audio files connected to AudioEnum
//-----------------------------------------------------------------------------
bool AudioManager::LoadAllAudio()
{
    // Loads audio for every entry in AudioEnum
    int enumCount = static_cast<int>(AudioEnum::AUDIO_ENUM_COUNT);
    for (int i = 0; i < enumCount; i++)
    {
        // Stops loading audio if something fails
        if (!LoadAudio(static_cast<AudioEnum>(i)))
            return false;
    }

    return true;
}


//-----------------------------------------------------------------------------
// Creates a new audio channel and plays a Mix_Chunk, 
// specified by an AudioEnum, on it
//-----------------------------------------------------------------------------
int AudioManager::Play(AudioEnum audioID)
{
    Mix_Chunk*& audioChunk = m_audioChunks[static_cast<int>(audioID)];
    int shouldLoop = audioID == AudioEnum::Music ? -1 : 0; // Only thing that should loop is the music

    // Plays audio chunk on a new channel
    int channel = Mix_PlayChannel(-1, audioChunk, shouldLoop);
    if (channel == -1)
    {
        std::cerr << "Could not create audio channel because there are no avaiable audio channels!" << '\n';
        return 69;
    }

    // Groups channel for easier access later
    Mix_GroupChannel(channel, static_cast<int>(audioID));

    // Returns channel for later handling
    return channel;
}


//-----------------------------------------------------------------------------
// Stops audio playing on a specific channel
//-----------------------------------------------------------------------------
void AudioManager::Stop(int channel)
{
    Mix_HaltChannel(channel);
}


//-----------------------------------------------------------------------------
// Stops audio playing on grouped channels specified by an AudioEnum
//-----------------------------------------------------------------------------
void AudioManager::Stop(AudioEnum audioID)
{
    Mix_HaltGroup(static_cast<int>(audioID));
}


//-----------------------------------------------------------------------------
// Stops all playing audio
//-----------------------------------------------------------------------------
void AudioManager::StopAll()
{
    for (int i = 0; i < static_cast<int>(AudioEnum::AUDIO_ENUM_COUNT); i++)
    {
        Stop(static_cast<AudioEnum>(i));
    }
}


//-----------------------------------------------------------------------------
// Removes one Mix_Chunks specified by an AudioEnum
//-----------------------------------------------------------------------------
void AudioManager::UnloadAudio(AudioEnum audioID)
{
    Mix_Chunk*& audioChunk = m_audioChunks[static_cast<int>(audioID)];
    if (audioChunk != nullptr)
    {
        Mix_FreeChunk(audioChunk);
        audioChunk = nullptr;
    }
}


//-----------------------------------------------------------------------------
// Used to remove all Mix_Chunks when game ends
//-----------------------------------------------------------------------------
void AudioManager::UnloadAllAudio()
{
    int enumCount = static_cast<int>(AudioEnum::AUDIO_ENUM_COUNT);
    for (int i = 0; i < enumCount; i++)
    {
        UnloadAudio(static_cast<AudioEnum>(i));
    }
}


//-----------------------------------------------------------------------------
// Helper function to match AudioEnums to filepath strings,
// could replace with an unordered_map
//-----------------------------------------------------------------------------
std::string AudioManager::GetAudioFilepath(AudioEnum audioID) const
{
    switch (audioID)
    {
    case AudioEnum::Music:          return "music.wav";
    case AudioEnum::ShotgunShoot:   return "shotgunShoot.wav";
    case AudioEnum::ShotgunReload:  return "shotgunReload.wav";
    case AudioEnum::EnemyKilled:    return "enemyKilled.wav";
    case AudioEnum::AmmoPickedUp:   return "ammoPickedUp.wav";
    case AudioEnum::KeyPickedUp:    return "keyPickedUp.wav";
    case AudioEnum::GameOver:       return "gameOver.wav";
    default:
        if (audioID != AudioEnum::AUDIO_ENUM_COUNT) // Avoid error message for the count itself
        { 
            std::cerr << "Invalid AudioEnum passed to GetAudioFilepath: " << static_cast<int>(audioID) << '\n';
        }
        return "";
    }
}
