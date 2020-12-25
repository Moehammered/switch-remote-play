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
    - You should see your PC's IP address (*'Host IP: 192.168.XXX.XXX'* in green)
    ![Network Discovery](network-discovery.jpg)
    - If this doesn't work, or stays stuck on *'Searching'* please check [PC Usage -> Step 2] or [Switch Usage -> Step 0]
    ![Network Discovery](searching-network.jpg)

3. Press **'R'** to **connect**
    - Once connected, the default control mode is **'Mouse'** mode.
      - L = left click, R = right click, analogs for movement
    - **Hold** the **touch screen** or **'ZL+ZR+B'** for more than **3 seconds** to switch to PS4 controller mode. You'll hear a sound on your PC play.

4. To **stop** the stream, hold **'+'** button for more than 3 seconds
