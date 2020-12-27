# How-To Instructions

## Install

### Switch Install

1. Have **[Atmosphere](https://github.com/Atmosphere-NX/Atmosphere/releases/latest)** CFW on your Switch *(other firmwares are untested)*
2. Copy the *switch* folder to the root directory of your Switch SD card

### PC Install

1. Have Windows 10 64 bit

2. [Download and install **Screen Capture Recorder to Video Windows free v0.12.11**](https://github.com/rdp/screen-capture-recorder-to-video-windows-free/releases/tag/v0.12.11)

3. [Download and install **ViGEm Bus v1.16.116**](https://github.com/ViGEm/ViGEmBus/releases/tag/setup-v1.16.116)

4. Copy the *switch-remote-play-host* folder from the **PC** folder to anywhere you like (Downloads, Desktop, Documents, etc...)

## Usage

### PC Usage

0. Make sure your PC and your Switch are connected on the **same network**
    - If you have a **5GHz WiFi network** try to make sure your Switch and PC are connected to it

    ![Switch and PC network](switch-pc-network.png)

1. Make sure the *ffmpeg.exe* program is located in the same folder as the *switch-remote-play-host.exe*
    - *ffmpeg.exe* is used to stream the audio and video to your switch
    ![ffmpeg location](ffmpeg-loc.png)

2. Open the *switch-remote-play-host.exe* program
    - If you get a **Windows Firewall** prompt, make sure to **ALLOW** the program access
      - If you don't allow access, the Switch will be unable to connect. You will have to **manually** add *switch-remote-play-host.exe* to the **Windows Firewall** rules to fix this
    - Once it is open, you should see *"Ready to receive a connection from the switch..."* and *"Waiting for connection to server"*
    ![application firewall prompt](firewall-prompt.png)

### Switch Usage

0. Make sure **Flight Mode is OFF** and your Switch and PC are connected on the **same network**
    - If you have a **5GHz WiFi network** try to make sure your Switch and PC are connected to it

1. Open the *switch-remote-play* app
   ![Switch Remote Play app](srp-app.jpg)

2. Press **'L'** to start network discovery
    - **You should see your PC's IP address (*'Host IP: 192.168.XXX.XXX'* in green)**
    ![Network Discovery](network-discovery.jpg)
    - **And you should see your Switch's IP on the PC application**
    ![PC Switch Discovery](host-handshake-success.png)
    - **If this doesn't work**, or stays stuck on *'Searching'* please check the [firewall section](#firewall)
    ![Network Searching](searching-network.jpg)

3. Press **'R'** to **connect**
    - Once connected, the default control mode is **'Mouse'** mode.
      - L = left click, R = right click, analogs for movement
    - **Hold** the **touch screen** or **'ZL+ZR+B'** for more than **3 seconds** to switch to PS4 controller mode. You'll hear a sound on your PC play.

4. To **stop** the stream, hold **'+'** button for more than 3 seconds

## Help it's not working

### Firewall

Please confirm that the Windows Firewall rules **allows both ffmpeg and switch remote play host** to access your private (and/or public network if you're using McDonalds wifi haha)

![srph-firewall-rule](firewall-rules.png)

![ffmpeg-firewall-rule](firewall-rules-ffmpeg.png)

### Network

Please make sure your switch and PC are on the same network. If you try to perform a network search and your PC IP doesn't show up, then it means they're not on the same network or they're being blocked by Windows Firewall.

Please confirm by trying to perform the network discovery in the switch app. **You should see your Switch display your PC's IP and your PC display your Switch's IP.**

![Network Discovery](network-discovery.jpg)
![PC Switch Discovery](host-handshake-success.png)

If you perform the network discovery feature and you don't see your Switch's IP on the PC application, Windows Firewall is blocking it, you're on a different network than your PC, your router is blocking the ports, or a PC app is already using the ports 19999 - 20004 (until the ports are configurable, you will need to find the app using those ports and stop it).

### Stuck on Pending Connection

Please make sure ffmpeg is not being blocked by Windows Firewall.

![ffmpeg-firewall-rule](firewall-rules-ffmpeg.png)

Please make sure that **if you're using manual IP mode**, that your **Switch and PC are on the same network** and that your PC IP is correct and that ffmpeg is not being blocked by Windows Firewall.

![manual-ip-mode](manual-ip-screen.jpg)

### Virus Threat Detected

My PC doesn't trigger or treat the program as a thread. Probably because I have Visual Studio installed. When I check it online there are some online file scanners that flag it as a virus. The release build seems to trigger more flags than the debug build.

My guess is due to the obfescation and the fact this program:

- keeps a socket open for broadcast listening (but I limit this to only the local subnet/network)
  - *windows-project -> main.cpp -> receiverProcedure*
- this program opens another process in the background(ffmpeg).
  - *windows-project -> FFMPEGHelper.cpp -> StartStream + StartAudio*

The source code is available and you are free to compile it yourself and check by downloading Visual Studio 2019.

If you can trust it then you will have to manually allow the program and get it out of quarentine with Windows Defender. And you **don't need to run this program as admin**.

If anyone with any insight can look at the source code and see what is causing it that would be great.
