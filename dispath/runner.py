import subprocess as sb

class BaseRunner(object):
    def __init__(self, command, arguments):
        self._c = command
        self._args = arguments
        self._status = "0"
    def __str__(self):
        return "The command is : " + self._c +
               "The arguments is: " + " ".join(self._args)
    def __repr__(self):
        return self.__str__()
    
    @static
    def runCommand(self, command, arguments):
        tp = sb.run(command + " ".join(arguments), stdout=sb.PIPE, shell=True)
        return tp.stdout.read()
    
    def run(self):
        return self.runCommand(self, self._c, self._args)

class HdlGenRunner(BaseRunner):
    def __init__(self, task):
        command = "coeff2code"
    def r():
        return super().run()

