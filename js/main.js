const path = require("path");
const vm = require("vm");
const utils = require("./utils")
const shmFd = utils.createSharedMemory();
async function runFunction(obj) {
    utils.writeToSharedMemory(shmFd, "");
    const filePath = path.resolve(__dirname, obj.path);
    console.log(filePath);
    const script = new vm.Script(`
        const obj = require("${filePath}");
        const func = obj.${obj.name};
        console.log(func)
        const result = func(...${JSON.stringify(obj.args)});
        result;
    `);

    const context = {
        require,
        console,
        JSON,
        setTimeout,
        setInterval,
        setImmediate,
    };
    const res = script.runInNewContext(context)
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
