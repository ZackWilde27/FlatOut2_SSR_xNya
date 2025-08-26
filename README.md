# FlatOut 2 Screen-Space Reflections (xNya)

This version of the mod is built for the xNya mod loader

The Reloaded version can be found [here](https://github.com/ZackWilde27/FlatOut2ScreenSpaceReflections)

Now it's a literal remake of Chloe's SSR mod

It does the exact same thing as the reloaded mod, so I'm re-using the demo video

https://github.com/user-attachments/assets/39fbc000-9f3d-422a-bfe2-121309c354f3

Just like with the reloaded mod, it only replaces the car body shader, I'll add the window shaders once I get around to those

<br>

## Installing
Copy all files in the zip to the game's folder, then add ```SSRShader.bfs``` on its own line to the ```patch``` file

<br>

## Building
### For the DLL
I used Visual Studio 2022 on Windows

You'll need Chloe's nya-common library, which can be cloned from [here](https://github.com/gaycoderprincess/nya-common)

### For the shaders
You'll need my [HLSL compiler](https://github.com/ZackWilde27/FlatOut2-HLSLToSHA)
