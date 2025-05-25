# Opiumism 2D

A short 2D game project inspired by *Hotline Miami*, written in C++ using the SDL3 framework. This project explores gameplay systems like raycasting, modular level loading, and simple state management.

![Language](https://img.shields.io/badge/language-C%2B%2B-orange)
![Framework](https://img.shields.io/badge/framework-SDL3-lightgrey)

## Dependencies

All dependencies are automatically downloaded using CMake FetchContent the first time you configure the project. This works both when opening the project in an IDE like Visual Studio or CLion, and when using the command line with: `cmake -B build`

- Made using the C++20 standard
- Requires CMake 3.16 or higher
- [SDL3](https://github.com/libsdl-org/SDL)
- [SDL_ttf](https://github.com/libsdl-org/SDL_ttf)
- [SDL_mixer](https://github.com/libsdl-org/SDL_mixer)
- [rapidJSON](https://github.com/Tencent/rapidjson)

---

## Planning
I want to make a short game inspired by Hotline Miami. I recently watched a lecture on how 2D raycasting works and found a [website](https://ncase.me/sight-and-light/) with a practical example of it. I'm not yet sure how I’ll implement this from a gameplay perspective. One thing I do know is that I want to include several levels, and I’d like to design the game in a way that allows me to modularly add new levels with (some) ease.

As I’m interested in game programming, I’ve decided to build the game using C++ and the SDL3 framework (since C++ is the industry standard for game development). I hope to earn an A on this project.


## Evaluation

### How the program works

The program consists of several components, and I think it would just be easier to read the code in order to understand it. I’ve added extensive comments throughout the code to make it easier to understand how everything works.

### What have you included from the planning?

I believe I’ve incorporated most of what I intended in the planning phase. I ended up using raycasting both for enemy line-of-sight and for the shotgun mechanic—though in hindsight, using projectiles for the shotgun might have been a better choice.

Initially, I planned to use only SDL3, but I realized that doing everything with just SDL3 would be too difficult. I expanded the stack to include:

- **SDL_ttf** (for rendering text)

- **SDL_mixer** (for handling audio)

- **rapidJSON** (for reading level JSON files)

### If something hasn't been included, why?

One feature I had planned but didn’t implement was doors—like those in Hotline Miami—which would block enemy sight lines. The doors were also supposed to rotate around a hinge when pushed by the player and be destructible. However, after implementing the core game objects, I found this feature too complex and time-consuming to add.

Another feature I dropped was using textures for everything (walls, enemies, the player, etc.). I didn’t use a tile map, which made texture placement difficult. In hindsight, I could have created a large 1920x1080 repeating texture and sliced it, but at this point, I lacked the motivation to pursue it.

Lastly, I intended to implement a proper, standalone Game Over screen class. Instead, I opted for a quick hack that does the job.


### What could you have done better?

I could have tried implementing an Entity-Component-System (ECS) architecture. That would have made the project easier to manage and more scalable.

My CMake setup is also not ideal. I didn’t take the time to properly learn the CMake API and instead relied on AI-generated code. As a result, I had to reload my configuration every time I changed assets or level files, which was tedious and rather time consuming.

Non-programming elements—such as level design, audio, and game balance—could definitely be improved. I found myself losing interest and motivation after finishing the core programming, which made these aspects suffer. I’ve learned that pure level design and balancing isn’t something I particularly enjoy.


### Was there anything that you thought turned out particularly well?

I'm really proud of my header-only 2D vector class. I tried to make it as clean and optimized as possible, and I think I succeeded. Some might argue I should have used a template to make it more flexible for other data types, but for a project that only needed `Vec2<float>`, that felt unnecessary.

I'm also proud of my use of a bitset to track picked-up items and dead enemies. Initially, I used an array of booleans, but I realized a single bit per item would suffice. Switching to a `std::bitset<>` using 65,536 bits (matching the number of values an unsigned 16-bit integer can hold) per object type reduced the memory usage by a factor of eight.

Finally, my Raycast class and Primitives2D namespace functions are well implemented. However, since much of that code was based on tutorials, I can't fully take credit for it.

---