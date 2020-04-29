import json

def format_itemname(item):
    return item.replace(' ', '_').replace('/', '-')

class Item(object):
    def __init__(self, name, wikipage='', iconurl='', iconpath='', itemtype=''):
        self.name = name
        self.wikipage = wikipage
        self.iconurl = iconurl
        self.iconpath = iconpath
        self.itemtype = itemtype

    def as_dict(self):
        return {
            format_itemname(self.name): {
                'realname': self.name,
                'wikipage': self.wikipage,
                'iconurl' : self.iconurl,
                'iconpath': self.iconpath,
                'itemtype': self.itemtype
            }
        }

    def export(self):
        return self.name, self.as_dict()

    def __str__(self):
        return json.dumps(self.as_dict(), indent=4, sort_keys=True)

    def __add__(self, other):
        if isinstance(other, dict):
            return {**self.as_dict(), **other}
        elif isinstance(other, Item):
            return {**self.as_dict(), **other.as_dict()}

    def __repr__(self):
        return str(self)

def debug():
    roub = Item('Roubles',
            wikipage='https://escapefromtarkov.gamepedia.com/Roubles',
            iconurl='https://gamepedia.cursecdn.com/escapefromtarkov_gamepedia/6/62/Roublesicon.png?version=1ddd57a1b36db139502eb137335569c9')
    eur = Item('Euros',
            wikipage='https://escapefromtarkov.gamepedia.com/Euros',
            iconurl='https://gamepedia.cursecdn.com/escapefromtarkov_gamepedia/f/f3/Eurosicon.png?version=cc9e22e64df6f3dfcbd9c508fd84c906')
    dol = Item('Dollars',
            wikipage='https://escapefromtarkov.gamepedia.com/Dollars',
            iconurl='https://gamepedia.cursecdn.com/escapefromtarkov_gamepedia/c/c4/Dollarsicon.png?version=f31d6c29f5a7beec810ae9dae23efc6e')
    itemdict = roub + eur
    print(json.dumps(dol + itemdict, indent=4, sort_keys=True))

if __name__ == '__main__':
    debug()
