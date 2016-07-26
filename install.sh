echo Install dependencies
echo Cinder-Hap2
git clone https://github.com/videodromm/Cinder-Hap2 ../../Cinder/blocks/Cinder-Hap2
echo Cinder-Videodromm
git clone https://github.com/videodromm/Cinder-Videodromm ../../Cinder/blocks/Cinder-Videodromm
echo Cinder-Warping
git clone https://github.com/paulhoux/Cinder-Warping ../../Cinder/blocks/Cinder-Warping
echo Cinder-ImGui
git clone https://github.com/simongeilfus/Cinder-ImGui ../../Cinder/blocks/Cinder-ImGui --recursive
echo Cinder-MIDI2
git clone https://github.com/brucelane/Cinder-MIDI2 ../../Cinder/blocks/Cinder-MIDI2
echo Cinder-WebSocketPP
git clone https://github.com/videodromm/Cinder-WebSocketPP ../../Cinder/blocks/Cinder-WebSocketPP
echo Cinder-WebSocketPP
git clone https://github.com/videodromm/assets ..
echo Required dependencies checked out
echo Pull changes
git pull
cd ../../Cinder/blocks/Cinder-Hap2
git pull
cd ../Cinder-Videodromm
git pull

