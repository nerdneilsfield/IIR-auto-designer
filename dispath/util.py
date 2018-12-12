import toml

class Config(object):
    def __init__(self, path):
        with open(path, 'r') as file:
            self.configs = toml.load(file.read())
    def __getitem__(self, i):
        return self.configs[i]
    def __len__(self):
        return len(self.configs)
    def __str__(self):
        return self.configs.__str__()
    def __repr__(self):
        return self.configs.__repr__()


        
