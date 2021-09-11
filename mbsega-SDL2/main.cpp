#include <iostream>
#include <fstream>
#include <cassert>
#include <libmbsega/libmbsega.h>
#include <SDL2/SDL.h>
using namespace sega;
using namespace std;

class SDL2Frontend : public mbsegaFrontend
{
    public:
	SDL2Frontend(MasterSystemCore &cb) : core(cb)
	{

	}

	~SDL2Frontend()
	{

	}

	bool init()
	{
	    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	    {
		return sdlerror("SDL2 could not be initialized!");
	    }

	    window = SDL_CreateWindow("mbsega-SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);

	    if (window == NULL)
	    {
		return sdlerror("Window could not be created!");
	    }

	    render = SDL_CreateRenderer(window, -1, 0);

	    if (render == NULL)
	    {
		return sdlerror("Renderer could not be created!");
	    }

	    texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 256, 240);

	    if (texture == NULL)
	    {
		return sdlerror("Texture could not be created!");
	    }

	    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);

	    SDL_AudioSpec audiospec;
	    audiospec.format = AUDIO_S16SYS;
	    audiospec.freq = 48000;
	    audiospec.channels = 2;
	    audiospec.samples = 4096;
	    audiospec.callback = NULL;

	    if (SDL_OpenAudio(&audiospec, NULL) < 0)
	    {
		return sdlerror("Could not open audio!");
	    }

	    SDL_PauseAudio(0);
	    return true;
	}

	void shutdown()
	{
	    SDL_CloseAudio();
	    if (texture != NULL)
	    {
		SDL_DestroyTexture(texture);
		texture = NULL;
	    }

	    if (render != NULL)
	    {
		SDL_DestroyRenderer(render);
		render = NULL;
	    }

	    if (window != NULL)
	    {
		SDL_DestroyWindow(window);
		window = NULL;
	    }

	    SDL_Quit();
	}

	void runapp()
	{
	    while (!quit)
	    {
		core.runcore();
		pollevents(quit);
		renderpixels();
		limitframerate();
	    }
	}

	void limitframerate()
	{
	    framecurrenttime = SDL_GetTicks();

	    if ((framecurrenttime - framestarttime) < 16)
	    {
	    	SDL_Delay(16 - (framecurrenttime - framestarttime));
	    }

	    framestarttime = SDL_GetTicks();

	    fpscount++;

	    if (((SDL_GetTicks() - fpstime) >= 1000))
	    {
	    	fpstime = SDL_GetTicks();
		stringstream title;
	    	title << "mbsega-SDL2-" << fpscount << " FPS";
		SDL_SetWindowTitle(window, title.str().c_str());
		fpscount = 0;
	    }
	}

	vector<uint8_t> loadfile(string filename)
	{
	    vector<uint8_t> result;

	    fstream file(filename.c_str(), ios::in | ios::binary | ios::ate);

	    if (file.is_open())
	    {
		streampos size = file.tellg();
		result.resize(size, 0);
		file.seekg(0, ios::beg);
		file.read((char*)result.data(), size);
		file.close();
	    }

	    return result;
	}

	void audiocallback(int16_t left, int16_t right)
	{
	    audiobuffer.push_back(left);
	    audiobuffer.push_back(right);

	    if (audiobuffer.size() >= 4096)
	    {
		audiobuffer.clear();

		while (SDL_GetQueuedAudioSize(1) > (4096 * sizeof(int16_t)))
		{
		    SDL_Delay(1);
		}

		SDL_QueueAudio(1, audiobuffer.data(), (4096 * sizeof(int16_t)));
	    }
	}

	void renderpixels()
	{
	    assert(render && texture);
	    SDL_UpdateTexture(texture, NULL, core.getframebuffer().data(), (256 * sizeof(segaRGB)));
	    SDL_RenderClear(render);
	    SDL_RenderCopy(render, texture, NULL, NULL);
	    SDL_RenderPresent(render);
	}

	void pollevents(bool &quit)
	{
	    while (SDL_PollEvent(&event))
	    {
		switch (event.type)
		{
		    case SDL_QUIT: quit = true; break;
		    case SDL_KEYDOWN:
		    case SDL_KEYUP:
		    {
			bool is_keydown = (event.type == SDL_KEYDOWN);

			switch (event.key.keysym.sym)
			{
			    case SDLK_a: keychanged(SMSButton::One, is_keydown); break;
			    case SDLK_b: keychanged(SMSButton::Two, is_keydown); break;
			    case SDLK_UP: keychanged(SMSButton::Up, is_keydown); break;
			    case SDLK_DOWN: keychanged(SMSButton::Down, is_keydown); break;
			    case SDLK_LEFT: keychanged(SMSButton::Left, is_keydown); break;
			    case SDLK_RIGHT: keychanged(SMSButton::Right, is_keydown); break;
			}
		    }
		    break;
		}
	    }
	}

	void keychanged(SMSButton button, bool is_pressed)
	{
	    if (is_pressed)
	    {
		core.keypressed(button);
	    }
	    else
	    {
		core.keyreleased(button);
	    }
	}

	bool sdlerror(string message)
	{
	    cout << message << " SDL_Error: " << SDL_GetError() << endl;
	    return false;
	}

	SDL_Window *window = NULL;
	SDL_Renderer *render = NULL;
	SDL_Texture *texture = NULL;

	bool quit = false;
	SDL_Event event;

	MasterSystemCore &core;

	int fpscount = 0;
	Uint32 fpstime = 0;

	Uint32 framecurrenttime = 0;
	Uint32 framestarttime = 0;

	vector<int16_t> audiobuffer;
};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
	cout << "Usage: mbsega-SDL2 [ROM]" << endl;
	return 1;
    }

    string romname = argv[1];

    MasterSystemCore core;
    SDL2Frontend *front = new SDL2Frontend(core);
    core.setfrontend(front);

    if (!core.loadROM(romname))
    {
	return 1;
    }

    if (!core.initcore())
    {
	return 1;
    }

    core.runapp();
    core.shutdown();
    return 0;
}