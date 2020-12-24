# switch-remote-play

Let the switch remotely play PC games (similar to steam link or remote play)

This project is inspired by the github project [In-Home-Switching](https://github.com/jakibaki/In-Home-Switching) and [SkyNX](https://github.com/DevL0rd/SkyNX). The goal is to make a convenient streamer/client application to be used to remote play PC games on the switch.

[In-Home-Switching](https://github.com/jakibaki/In-Home-Switching) and [SkyNX](https://github.com/DevL0rd/SkyNX) already offer the ability to do so, however there were some convenience features missing that I wanted to add. I also recently was studying multi-threading and when I saw the code, I thought 'this is the perfect chance to try writing it in C++'.

## Switch-Project

This project consists of an application written for the switch that will decode stream data received from [FFMPEG](https://github.com/FFmpeg/FFmpeg) and send its input to an application to emulate a controller. The rendering is done with SDL2 and the project is written in C++.

- **Tested on Atmosphere 0.14.1 - FW 10.1.0**

## Windows-Project

This project consists of an application written for Windows that will launch an FFMPEG stream to encode and send data. The application, after launching the [FFMPEG](https://github.com/FFmpeg/FFmpeg) stream, will then listen to receive input data and emulate a controller via the [ViGEm Client SDK](https://github.com/ViGEm/ViGEmClient). The [ViGEm Client SDK](https://github.com/ViGEm/ViGEmClient) expects the [ViGEm Bus Kernel Driver](https://github.com/ViGEm/ViGEmBus) to be installed on the machine in order to work. For more info on the kernel driver code see the [ViGEmBus](https://github.com/ViGEm/ViGEmBus) github page. For the driver installation, see the [ViGEmBus Releases](https://github.com/ViGEm/ViGEmBus/releases) page.

The Windows project is also written in C++ and uses Winsock to do socket communication with the switch application.

## Features

- [x] No system crashing when going to sleep (stream will disconnect though and application is closeable)
- [x] Desktop application to host remote connection stream
- [x] Stream input from the switch to remote connection
- [x] Stream audio from remote connection to the switch
- [x] Automatically connect switch and PC stream applications
- [x] Allow manual configuration of switch to PC connection
- [x] Configure stream settings from switch
- [x] Disconnect / Reconnect stream at will
- [x] Toggle Input Mode to Mouse
- [x] Toggle Input Mode to DS4(PS4) controller
- [ ] Toggle Input Mode to Xbox controller

## Known Issue

After closing a stream and then closing the application. If the homebrew loader menu/app is not closed, any app opened next will either crash or lock up and will require the homebrew menu to be closed via the home button.

To reproduce:

- Successfully connect to a stream from your PC with switch-remote-play
- Stop the stream
  - Either by closing the application on the PC or holding '+' on the switch
- Close the switch-remote-play app with the '+' button
- You are back in the homebrew menu. Open any homebrew app (Goldleaf or switch-remote-play for example)
- The app will freeze or crash

To avoid this, just close the homebrew loader with the HOME button. I'm still trying to figure out why it's happening. If anyone has any insight please contact me.

## Thanks

- [jakibaki](https://github.com/jakibaki) for the original [In-Home-Switching](https://github.com/jakibaki/In-Home-Switching) switch application
- [DevL0rd](https://github.com/DevL0rd) for the updated streaming application [SkyNX](https://github.com/DevL0rd/SkyNX)
- [FFmpeg](https://www.ffmpeg.org/) for an exhaustingly extensive library
- [nefarius](https://github.com/nefarius) for the [ViGEm Client SDK](https://github.com/ViGEm/ViGEmClient). It's such a nice SDK/library to use
- [rdp](https://github.com/rdp) for [screen-capture-recorder](https://github.com/rdp/screen-capture-recorder-to-video-windows-free). Made streaming audio via FFmpeg on PC very easy
- [SwitchBrew](https://switchbrew.org/wiki/Main_Page) for [libNX](https://github.com/switchbrew/libnx)
- [devkitPro](https://devkitpro.org/) bloody legends for letting us have a toolchain to compile stuff for the Switch
- [SciresM, TuxSH, hexkyz, and fincs](https://github.com/Atmosphere-NX/Atmosphere) for Atmosphere. Absolute madlads these lot are. Also a shoutout to the champs that help contribute to the repo.
- Jonathan Dearborn for SDL_FontCache

## Licence

GPLv3, so please follow the open source licence and be a nice bloke. If you use parts of this project or expand on this project, make it open source. Cheers mate. (You can read more about the GPLv3 licence [HERE](https://tldrlegal.com/license/gnu-general-public-license-v3-(gpl-3)) and [HERE](https://choosealicense.com/licenses/lgpl-3.0/))

The [ViGEm Client SDK](https://github.com/ViGEm/ViGEmClient) uses an [MIT](https://tldrlegal.com/license/mit-license) licence and it has been included with the Windows project. (The Windows project itself is still licensed under [GPLv3](https://tldrlegal.com/license/gnu-general-public-license-v3-(gpl-3)))
