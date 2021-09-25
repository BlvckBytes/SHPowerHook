USR=`whoami`
BIN_PATH=/Users/$USR/Library/Developer/Xcode/DerivedData/SHPowerHook-*/Build/Products/Debug
OUT_PATH=/Library/Extensions

sudo rm -rf $OUT_PATH/SHPowerHook.kext
sudo cp -R $BIN_PATH/SHPowerHook.kext $OUT_PATH/ 
