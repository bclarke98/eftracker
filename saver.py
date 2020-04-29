#!/usr/bin/env python3
import os
import sys
import json
import requests

from multiprocessing import Pool
from scraper import mkdir

def download_icons():
    mkdir('img')
    try:
        icons = {}
        with open('dat/iconcache.json', 'r') as j:
            icons = json.loads(j.read())
        amtdl = 0
        for k in icons:
            r = requests.get(icons[k])
            if r.status_code == 200:
                with open(f'img/{k}.png', 'wb') as f:
                    f.write(r.content)
                amtdl += 1
                print(f'Downloaded {amtdl}/{len(icons)} icons.', end='\r')
            else:
                print(f'Unable to download icon for "{k}".')
    except FileNotFoundError as e:
        print(e)
        print('No "dat" directory found. Please run "scraper.py" first.')

G_ICONS = None

def dl_icon(name):
    r = requests.get(G_ICONS[name])
    if r.status_code == 200:
        with open(f'img/{name}.png', 'wb') as f:
            f.write(r.content)
    else:
        print(f'Unable to download icon for "{name}".')


def m_download_icons():
    global G_ICONS
    mkdir('img')
    try:
        G_ICONS = {}
        with open('dat/iconcache.json', 'r') as j:
            G_ICONS = json.loads(j.read())
        amtdl = 0
        with Pool() as p:
            p.map(dl_icon, [i for i in G_ICONS])
        print('Finished downloading icons.')
    except FileNotFoundError as e:
        print(e)
        print('No "dat" directory found. Please run "scraper.py" first.')

def main():
    m_download_icons()

if __name__ == '__main__':
    main()


