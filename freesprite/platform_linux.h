#pragma once

//I gave up on trying to make this work
//first sdl2 was too old
//then sdl2_image was too old
//then g++ was one version too old to support #include <format>
//then the png.h include threw 500 errors


//Good fucking luck i can't be bothered with this shit

void platformPreInit() {

}
void platformInit() {
	
}
void platformPostInit() {
	
}

void platformTrySaveImageFile(EventCallbackListener* listener) {
    
}

FILE* platformOpenFile(PlatformNativePathString path, PlatformNativePathString mode) {
    FILE* ret = fopen(path.c_str(), mode.c_str());
    return ret;
}