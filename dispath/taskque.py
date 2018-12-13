import json
import uuid
import time
import redis

class Task:
    def __init__(self, json_str=""):
        if json_str == "":
            self.id = uuid.uuid1().hex
            self.L = {
                "id" : "", # id 
                "dir": "", # operation dir
                "action":"", # action
                "next":"", # next action
                "status": "", # status: finishing wait
                "worker": "", # worker: worker id
                "inputs": [], # inputs: input args
                "outputs":[], # outputs: output args
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
        self.L["worker"] = worker
    def getID(self):
        return self.id
    def setAction(self, action):
        self.L["action"] = action
    def getAction(self):
        return self.L["action"]
    def setNextAction(self, nextAction):
        self.L["next"] = nextAction
    def getNextAction(self):
        return self.L["next"]
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

    def getLists(self, start=0, end=20, filter=None):
        """getLists
        
        :param start:int start index 
        :param end:int stop index
        :param filter:a lambda function for filter output data
        """
        
        task_ids = self.r.lrange(self.configs["taskQueue"]["processList"], start, end)
        tasks = []
        for task_id in task_ids:
            tasks.append(self.r.get(task_id))
        return tasks

    def updateTask(self, task_id, task_str):
        """updateTask
        if last_status and this_status not equal: update task.
        if next_status is not empty append a new task to task queue
        :param task_id:string(uuid) the id of task_id
        :param task_str:string(json_str) the str(task)
        """
        if self.r.exists(task_id):
            task_str_before = self.r.get(task_id)
            task_before = Task(task_str_before)
            task_after = Task(task_str)
            if task_before.getStaus() != task_after.getStaus():
                self.r.set(task_id, task_str)
                if task_after.getStaus == "finish":
                    if task_after.getNextAction != "":
                        pass
        else:
            raise TaskQueue

