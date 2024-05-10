const utils = require("./utils")
const shmFd = utils.createSharedMemory();
async function runFunction(obj) {
    utils.writeToSharedMemory(shmFd, "");
    res = 2;
    const result = {
        result: res,
        op: "response",
        uuid: obj.uuid,
    };
    utils.writeToSharedMemory(shmFd, JSON.stringify(result));
}

const obj = JSON.parse(utils.readFromSharedMemory(shmFd) || '{}');
console.log(obj)
runFunction(obj);
