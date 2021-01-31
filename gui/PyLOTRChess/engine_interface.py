from subprocess import Popen, PIPE

# https://stackoverflow.com/questions/36476841/python-how-to-read-stdout-of-subprocess-in-a-nonblocking-way
class EngineInterface:
    def __init__(self, path):
        self.engine_path_ = path # '/Users/zachbortoff/Projects/Cloak/cmake-build-debug/cloak'
        self.engine_proc_ = Popen(self.engine_path_, bufsize = 0, stdin = PIPE, stdout = PIPE)