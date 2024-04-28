const ffi = require("ffi-napi");

const sharedLib = ffi.Library("/app/shared/shared.so", {
    create_shared_memory: ["int", []],
    write_to_shared_memory: ["void", ["int", "string"]],
    read_from_shared_memory: ["string", ["int"]],
});

function createSharedMemory() {
    return sharedLib.create_shared_memory();
}

function writeToSharedMemory(shmFd, data) {
    sharedLib.write_to_shared_memory(shmFd, data);
}

function readFromSharedMemory(shmFd) {
    return sharedLib.read_from_shared_memory(shmFd);
}

module.exports = {
    createSharedMemory,
    writeToSharedMemory,
    readFromSharedMemory,
};
