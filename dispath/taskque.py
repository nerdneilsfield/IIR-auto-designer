import json
import uuid
import time
import redis

class Task:
    def __init__(self, json_str=""):
        if json_str == "":
            self.id = uuid.uuid1().hex
            self.L = {
                "id" : "",
                "dir": "",
                "status": "",
                "worker": "",
                "inputs": "",
                "outputs":"",
                "create_time": time.time(),
                "update_time": time.time(),
            }
            self.L["id"] = self.id
        else:
            self.L = json.loads(json_str)
            self.id = self.L["id"]
    def __str__(self):
        return self.L.__str__()
    def __repr__(self):
        return self.L.__repr__()
    def str(self):
        self.L["update.time"] = time.time()
        return json.dumps(self.L)
    def getStaus(self):
        return self.L["status"]
    def setStatus(self, status):
        self.L["status"] = status
    def setInputArgs(self, args):
        self.L["inputs"] = args
    def setOutputArgs(self, args):
        self.L["outputs"] = args
    def getInputArgs(self):
        return self.L["inputs"]
    def getOuputArgs(self):
        return self.L["outputs"]
    def setDirPath(self, path):
        self.L["dir"] = path
    def getDirPath(self):
        return self.L["dir"]
    def getWorker(self):
        return self.L["worker"]
    def setWorker(self, worker):
        return self.L["worker"]
    def getID(self):
        return self.id
    def __getitem__(self, i):
        return self.L.__getitem__(i)
    def __setitem__(self, i, value):
        return self.L.__setitem__(i, value)

class TaskQueueError(Exception):
    pass

class TaskQueue:
    def __init__(self, configs):
        self.configs = configs
        self.r = redis.Redis(host=configs["redis"]["host"],
                             port=configs["redis"]["port"], 
                             decode_responses=True, 
                             password=configs["redis"]["password"])
    def addTask(self, task):
        task_id = task.getID()
        task_str = task.str()
        self.r.lpush(self.configs["taskQueue"]["taskList"], task_id)
        self.r.lpush(self.configs["taskQueue"]["processList"], task_id)
        self.r.set(task_id, task_str)

    def accepetTask(self):
        task_id = self.r.brpop(self.configs["taskQueue"]["tasklist"])
        task_str = self.r.get(task_id)
        return (task_id, task_str)

    def getLists(self, start=0, end=20):
        task_ids = self.r.lrange(self.configs["taskQueue"]["processList"], start, end)
        tasks = []
        for task_id in task_ids:
            tasks.append(self.r.get(task_id))
        return tasks

    def updateTask(self, task_id, task_str):
        if self.r.exists(task_id):
            self.r.set(task_id, task_str)
        else:
            raise TaskQueue

