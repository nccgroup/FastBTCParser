#include "file_type_db.h"

static std::map<std::string, std::string> extensionMap;

const char* fileTypes[FILE_TYPE_SIZE] = {
"application/epub+zip",
"application/javascript",
"application/json",
"application/octet-stream",
"application/pdf",
"application/pgp-signature",
"application/x-gzip",
"application/yaml",
"audio/flac",
"audio/midi",
"audio/mod",
"audio/mpeg",
"audio/ogg",
"audio/wav",
"dadabots/was+here",
"image/avif",
"image/gif",
"image/jpeg",
"image/jpeg;charset=utf-8",
"image/png",
"image/svg+xml",
"image/tiff",
"image/webp",
"model/gltf-binary",
"model/stl",
"text/html;charset=utf-8",
"text/javascript",
"text/markdown",
"text/plain",
"text/plain;charset=us-ascii",
"text/plain;charset=utf-8",
"text/plainn;charset=utf-",
"text/plainn;charset=utf-8",
"video/mp4",
"video/webm"
};

const char* fileExtension[FILE_TYPE_SIZE] = {
"epub",
"js",
"json",
"bin",
"pdf",
"pgp",
"gz",
"yaml",
"flac",
"midi",
"mod",
"mpeg",
"ogg",
"wav",
"bin",
"avif",
"gif",
"jpg",
"jpg",
"png",
"svg",
"tiff",
"webp",
"glb",
"stl",
"html",
"js",
"md",
"txt",
"txt",
"txt",
"txt",
"txt",
"mp4",
"webm"
};


void initFileTypeDB() {
    if(extensionMap.size() == 0) {
        for(int i = 0; i < FILE_TYPE_SIZE; ++i) {
            extensionMap[fileTypes[i]] = fileExtension[i];
        }
    }
}

std::string getFileTypeExtension(std::string dataType) {
    return extensionMap[dataType];
}
