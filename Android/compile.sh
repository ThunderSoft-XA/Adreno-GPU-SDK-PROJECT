cd ../jni
$AND_NDK/ndk-build -B
./InstallAssets.sh
android update project -p . -t android-24
ant debug
adb install -r bin/NativeActivity-debug.apk 
