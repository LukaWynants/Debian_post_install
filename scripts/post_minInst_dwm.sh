#!/bin/bash

quit () {
  read -p "Abort? Y/N" antw
  if [ $atnw = 'y' ] || [ $antw = 'Y' ]; then
	  exit 1
  fi
} 


echo "###############"
echo "# basic Utils #"
echo "###############"

sudo apt install git build-essential make kitty firefox-esr pipewire python3 nmap tldr net-tools ntfs-3g keepassxc

echo "###################"
echo "# X11 xserver etc.#"
echo "###################"

quit

sudo apt install xserver-xorg-core xserver-xorg-video-intel xinit xinput x11-xserver-utils

echo '#############'
echo '# LibX etc. #'
echo '#############'

sudo apt install libx11-dev libxinerama-dev libxft-dev libpam0g-dev libxcb-xkb-dev




echo "################"
echo "# DWM suckless #"
echo "################"

cd ~
mkdir .config
cd .config/
git clone https://git.suckless.org/dwm
git clone https://git.suckless.org/dmenu

