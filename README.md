# switch-remote-play

Let the switch remotely play PC games (similar to steam link or remote play)

For instructions, please see the [instructions](https://github.com/Moehammered/switch-remote-play/blob/master/instructions/Instructions.md) folder in the repo.

This project is inspired by the github project [In-Home-Switching](https://github.com/jakibaki/In-Home-Switching) and [SkyNX](https://github.com/DevL0rd/SkyNX). The goal is to make a convenient streamer/client application to be used to remote play PC games on the switch.

[In-Home-Switching](https://github.com/jakibaki/In-Home-Switching) and [SkyNX](https://github.com/DevL0rd/SkyNX) already offer the ability to do so, however there were some convenience features missing that I wanted to add. I also recently was studying multi-threading and when I saw the code, I thought 'this is the perfect chance to try writing it in C++'.

## Note

The video stream is CPU dependent as at the moment it uses the ffmpeg option of *hwaccel auto* which usually results in ffmpeg picking CPU for the screen capture process. In my initial prototyping phase I found choosing the AMD GPU option for ffmpeg led to inconsistent stream results. I will look at making the hwaccel option configurable in the future but for now just keep in mind CPU is important.

If you're playing a game that is CPU intensive then there's a high chance the stream will have skips or hiccups. If your CPU is weak, then it'll be hard for it to produce and push the frames quick enough.

For my PC, when I play Nier Automata there are no hiccups and I can get near 60 fps depending on the stream settings (1280 x 720 desktop res, and 5mb/s bitrate). But when I tried playing RE3 2020 at 30 or 56fps I was getting hiccups and skips. So please keep this in mind.

## Features

- [x] No system crashing when going to sleep (stream will disconnect though and application is closeable)
- [x] Desktop application to host remote connection stream
- [x] Stream input from the switch to remote connection
- [x] Stream audio from remote connection to the switch
- [x] Automatically connect switch and PC stream applications
- [x] Automatically mute PC when audio is streaming to the Switch
- [x] Allow manual configuration of switch to PC connection
- [x] Configure stream settings from switch
- [x] Disconnect / Reconnect stream at will
- [x] Toggle Input Mode to Mouse
- [x] Toggle Input Mode to DS4(PS4) controller
- [ ] Toggle Input Mode to Xbox controller

## Switch-Project

This project consists of an application written for the switch that will decode stream data received from [FFMPEG](https://github.com/FFmpeg/FFmpeg) and send its input to an application to emulate a controller. The rendering is done with SDL2 and the project is written in C++.

Tested on:

- **Atmosphere 0.14.1 - FW 10.1.0** (switch-remote-play 0.7.2)
- **Atmosphere 0.16.2 - FW 10.1.0** (switch-remote-play 0.7.2)

## Windows-Project

This project consists of an application written for Windows that will launch an FFMPEG stream to encode and send data. The application, after launching the [FFMPEG](https://github.com/FFmpeg/FFmpeg) stream, will then listen to receive input data and emulate a controller via the [ViGEm Client SDK](https://github.com/ViGEm/ViGEmClient). The [ViGEm Client SDK](https://github.com/ViGEm/ViGEmClient) expects the [ViGEm Bus Kernel Driver](https://github.com/ViGEm/ViGEmBus) to be installed on the machine in order to work. For more info on the kernel driver code see the [ViGEmBus](https://github.com/ViGEm/ViGEmBus) github page. For the driver installation, see the [ViGEmBus Releases](https://github.com/ViGEm/ViGEmBus/releases) page.

The Windows project is also written in C++ and uses Winsock to do socket communication with the switch application.

Tested on:

- **Windows 10 64 bit**
  - Ryzen 5 2600 3.4GHz
  - AMD Radeon RX 5600 XT 6GB
  - 16GB RAM

## Known Issue

After closing a stream and then closing the application. If the homebrew loader menu/app is not closed, any app opened next can either crash or lock up and will require the homebrew menu to be closed via the HOME button.

To reproduce:

- Successfully connect to a stream from your PC with switch-remote-play
- Stop the stream
  - Either by closing the application on the PC or holding '+' on the switch
- Close the switch-remote-play app with the '+' button
- You are back in the homebrew menu. Open any homebrew app (Goldleaf or switch-remote-play for example)
- The app *might* freeze or crash

To avoid this, **just close the homebrew loader with the HOME button**. I know **when** it happens but I'm still trying to figure out why it's happening. If anyone has any insight please contact me or if you're curious, the crash point for switch-remote-play is in *SystemSetup.cpp -> LoadSystemFont -> TTF_OpenFont*.

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
