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
Get [Cinder, glNext branch](https://github.com/cinder/Cinder/tree/glNext), git clone with arguments: --recursive -b glNext
<p align="center">
  <img src="http://libcinder.org/images/logo.png" alt="Cinder Logo"/>
</p>
Get [Reymenta Cinder block, glNext branch](https://github.com/Reymenta-Visuals/Cinder-Reymenta/tree/glNext), git clone with arguments: -b glNext (Windows only)
<p align="center">
  <img src="http://www.batchass.fr/reymenta/reymenta-logo150.jpg" alt="Reymenta Logo"/>
</p>
Get [MinimalUI Cinder block, glNext branch](https://github.com/nselikoff/Cinder-MinimalUI/tree/glNext), git clone with arguments: -b glNext
<br />
Get [PureData](http://puredata.info/)
<p align="center">
  <img src="http://puredata.info/downloads/pd-extended/screenshot" alt="PureData screenshot"/>
</p>

Keyboard shortcuts
==================
- u: show/hide UI
- c: show/hide mouse cursor

OSC messages
============
Receiving on port 9001
Address: /live/track/meter Type: float Interval: from 0.0 to 1.0