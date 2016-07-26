AdamBrucePiotr
==============

Cinder project with Pure Data
<p align="center">
  <img src="http://www.batchass.fr/reymenta/abp.jpg" alt="Screenshot"/>
</p>

About
=====
A PureData patch creates music and sends data to the Cinder visuals software via OSC.
Several visual parameters like color, alpha, zoom, repetition of objects, tempo are given by the PureData patch for live interaction.
Music and PureData : Adam
Visuals developed by Bruce and Piotr

Setup
=====
Windows
- Install 32 bit visual studio 2013 redistribuable from http://www.microsoft.com/fr-fr/download/details.aspx?id=40784
- Copy the files from the setupPackage folders to the destination computer.
- Edit assets/Settings.xml to specify a OSC receiver port.
- Launch ABP.exe<br />
Mac
- Copy the files from the assets and ABP.app the setupPackage folder to the destination computer.
- Launch ABP.app

Run from the source
===================

Get [Cinder](https://github.com/cinder/Cinder),git clone Cinder --recursive
In the blocks folders clone:
- https://github.com/videodromm/Cinder-Videodromm.git
- https://github.com/mpcdigital/Cinder-Hap2.git (TODO can be removed on Mac, does not compile)
- https://github.com/paulhoux/Cinder-Warping.git
- https://github.com/videodromm/Cinder-WebSocketPP.git
- https://github.com/simongeilfus/Cinder-Imgui.git
- https://github.com/brucelane/Cinder-MIDI2.git
- https://github.com/brucelane/Cinder-Spout.git (Windows only)
- https://github.com/naychrist/Cinder-Syphon/tree/glNext (glNext branch, Mac only)

In the parent folder of Cinder create a folder named AVUIs and git clone https://github.com/AVUIs/AdamBrucePiotr

Get [PureData](http://puredata.info/)
<p align="center">
  <img src="http://www.batchass.fr/reymenta/puredata.jpg" alt="PureData patch"/>
</p>

Keyboard shortcuts
==================
- c: show/hide mouse cursor

OSC messages
============
Receiving on port 9001
Address: /live/track/meter Type: float Interval: from 0.0 to 1.0
Address: /live/beat Type: int Interval: incremented from 0 (start)  