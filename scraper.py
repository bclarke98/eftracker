#!/usr/bin/env python3
import os
import sys
import json
import requests
from multiprocessing import Process, Pool
from lxml import html

from item import Item, format_itemname

WIKI_BASE = 'https://escapefromtarkov.gamepedia.com'

WIKI_CACHE = {}
CONTAINERS = None

def die(s):
    print('[-] ' + s)
    sys.exit(1)

def get_html_tree(url):
    return html.fromstring(requests.get(url).content)

def mkdir(path):
    try:
        os.mkdir(os.path.join(os.getcwd(), path))
    except FileExistsError:
        pass
    except:
        die(f'Cannot mkdir "{path}"')

def get_loot_table_urls():
    tree = get_html_tree(f'{WIKI_BASE}/Looting')
    containers = {}
    try:
        searchables = tree.xpath('//table[@id="Searchable_Containers_Table"]')[0]
    except IndexError:
        die('Unable to find loot table.')
    links = searchables.xpath('tbody//tr//th[2]//a')
    for link in links:
        try:
            href = link.xpath("attribute::href")[0]
            containers[href[1:]] = {'wiki': f'{WIKI_BASE}{href}'}
        except IndexError:
            pass
    return containers

def m_scrape_loot_table(c):
    global CONTAINERS
    tree = get_html_tree(CONTAINERS[c]['wiki'])
    try:
        table = tree.xpath('//table[contains(@class, "wikitable") and contains(@class, "sortable")]')[0]
    except IndexError:
        die('Unable to find loot table.')
    drops = table.xpath('tbody//tr//td[1]//a')
    r = {}
    ic = {}
    for i in drops:
        try:
            itemwiki = i.xpath('attribute::href')[0]
        except IndexError:
            continue
        name = itemwiki[1:]
        fmt = format_itemname(name)
        icon, officialname = get_item_icon(f'{WIKI_BASE}{itemwiki}')
        if icon:
            r[format_itemname(officialname)] = icon
            ic[fmt] = Item(officialname,
                                    iconurl=icon,
                                    wikipage=f'{WIKI_BASE}{itemwiki}')
    CONTAINERS[c]['drops'] = [i for i in r]
    print(f'[+] Cached drops for container "{c}"')
    return c, CONTAINERS[c], ic, r

def get_item_icon(url):
    try:
        tree = WIKI_CACHE[url]
    except KeyError:
        WIKI_CACHE[url] = get_html_tree(url)
    tree = WIKI_CACHE[url]
    try:
        td = tree.xpath('//table[@class="va-infobox-icon-table"]//td[@class="va-infobox-icon"]')[0]
        img = td.xpath('a//img/@src')[0]
        div = tree.xpath('//div[@class="va-infobox-title-main"]/text()')[0]
        return img, div
    except IndexError:
        return None, None

def multithread_cache_loot_tables():
    global CONTAINERS
    CONTAINERS = get_loot_table_urls()
    container_names = sorted([i for i in CONTAINERS])
    lootd = {}
    itemd = {}
    icond = {}
    with Pool() as p:
        m = p.map(m_scrape_loot_table, container_names)
        for c, d, itc, icc in m:
            lootd[c] = d
            itemd.update(itc)
            icond.update(icc)
    j = json.dumps(lootd, indent=4, sort_keys=True)
    with open('dat/droptable.json', 'w') as f:
        f.write(j)
    print('[+] Saved droptable data to "dat/droptable.json"')
    todel = [i for i in icond if icond[i] is None]
    for i in todel:
        del icond[i]
    ij = json.dumps(icond, indent=4, sort_keys=True)
    with open('dat/iconcache.json', 'w') as f:
        f.write(ij)
    print('[+] Saved cached wiki items to "dat/iconcache.json"')
    td = {}
    for i in itemd:
        td.update(itemd[i].as_dict())
    with open('dat/items.json', 'w') as f:
        f.write(json.dumps(td, indent=4, sort_keys=True))
    print('[+] Saved cached item data to "dat/items.json"')

def main():
    mkdir('dat')
    multithread_cache_loot_tables()
    #cache_loot_tables()

if __name__ == '__main__':
    main()

